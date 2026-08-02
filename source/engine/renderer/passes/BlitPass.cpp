#include "BlitPass.h"
#include "../../core/DeviceContext.h"
#include "../RenderContext.h"
#include "../RenderTarget.h"
#include "../../core/DepthBuffer.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../externals/d3dx12/d3dx12.h"
#include "pix3.h"

#include <cstdio>
#include <cstring>

extern DeviceContext deviceContext;
extern RenderContext renderContext;

namespace
{
	const char* FormatToString(DXGI_FORMAT format)
	{
		switch (format)
		{
		case DXGI_FORMAT_R8G8B8A8_UNORM: return "R8G8B8A8_UNORM";
		case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB: return "R8G8B8A8_UNORM_SRGB";
		case DXGI_FORMAT_R32_FLOAT: return "R32_FLOAT";
		case DXGI_FORMAT_R32_UINT: return "R32_UINT";
		case DXGI_FORMAT_R32G32_UINT: return "R32G32_UINT";
		case DXGI_FORMAT_R32_TYPELESS: return "R32_TYPELESS";
		case DXGI_FORMAT_D32_FLOAT: return "D32_FLOAT";
		default: return "<unknown>";
		}
	}

	const char* ModeToString(int mode)
	{
		switch (mode)
		{
		case 0: return "Color";
		case 1: return "Object IDs (uint)";
		case 2: return "Depth (grayscale)";
		case 3: return "Object IDs (uint bits in float)";
		default: return "<unknown>";
		}
	}

	UINT BytesPerPixel(DXGI_FORMAT format)
	{
		return format == DXGI_FORMAT_R32G32_UINT ? 8 : 4;
	}
}

BlitPass::BlitPass() : RenderPass(L"Blit", L"debug_blit.hlsl", Type::Compute)
{
}

BlitPass::~BlitPass()
{
}

void BlitPass::ConfigurePipelineState()
{
	RootSignatureBuilder builder;
	builder.AddConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Mode @ b0
	builder.AddUAVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // Output @ u0
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // Float source @ t0
	builder.AddSRVTable(1, 1, D3D12_SHADER_VISIBILITY_ALL); // Uint source @ t1
	rootSignature = renderContext.CreateRootSignature(builder);

	TextureCreateDesc textureDesc;
	textureDesc.width = 1920;
	textureDesc.height = 1080;
	textureDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.name = "DebugBlitTexture";
	textureDesc.createUav = true;
	textureDesc.staticSrv = true;
	textureDesc.staticUav = true;
	textureDesc.lifeSpan = APP;
	debugTexture = renderContext.CreateEmptyTexture(textureDesc);
}

void BlitPass::PostAssetLoad()
{
}

void BlitPass::Initialize()
{
}

void BlitPass::Update()
{
	const bool overrideActive = debugSourceIndex > 0
		&& debugSourceIndex <= static_cast<int>(debugSources.size());

	const HTexture texture = overrideActive
		? debugSources[debugSourceIndex - 1].texture
		: GetDefaultSourceTexture();
	const int mode = overrideActive
		? debugSources[debugSourceIndex - 1].mode
		: static_cast<int>(VisualizationMode::Color);

	const D3D12_RESOURCE_DESC desc = renderContext.GetTexture(texture)->GetResource()->GetDesc();
	const float sizeMB = static_cast<float>(desc.Width * desc.Height * BytesPerPixel(desc.Format))
		/ (1024.0f * 1024.0f);

	snprintf(sourceInfo, sizeof(sourceInfo),
		"Resolution: %u x %u\n"
		"Format: %s\n"
		"Mip levels: %u\n"
		"View: %s\n"
		"Size: %.1f MB (mip 0)",
		static_cast<UINT>(desc.Width), desc.Height,
		FormatToString(desc.Format),
		static_cast<UINT>(desc.MipLevels),
		ModeToString(mode),
		sizeMB);
}

void BlitPass::RegisterSettings(RenderPassSettings& settings)
{
	// BlitPass is the last pass in the render graph, so by the time this runs
	// every other pass has already created its render targets and depth buffers.
	debugSources.clear();
	debugSourceItems.clear();
	debugSourceItems.push_back("Default");

	const std::vector<RenderTarget*>& renderTargets = renderContext.GetRenderTargets();
	for (size_t i = 0; i < renderTargets.size(); ++i)
	{
		const RenderTarget* target = renderTargets[i];

		// Back buffers have no SRV and are the blit destination anyway.
		if (strcmp(target->GetName(), "RT_BackBuffer") == 0)
		{
			continue;
		}

		const DXGI_FORMAT format = target->GetFormat();
		if (format == DXGI_FORMAT_R32G32_UINT)
		{
			// debug_blit.hlsl declares SourceUint as a scalar Texture2D<uint>;
			// an R32G32_UINT SRV has two 32-bit channels and doesn't match
			// that declaration, so there's no safe way to visualize it yet.
			continue;
		}

		DebugSource source;
		source.texture = renderContext.GetTexture(HRenderTarget(i));
		if (format == DXGI_FORMAT_R32_UINT)
		{
			source.mode = VisualizationMode::UintId;
		}
		else if (format == DXGI_FORMAT_R32_FLOAT)
		{
			// RenderTargetFormat::R32_UINT maps to an R32_FLOAT resource, and
			// passes like Selection write raw uint IDs into it (readback picking
			// reinterprets the bytes, which is why picking works). Bitcast back.
			source.mode = VisualizationMode::UintBitcast;
		}
		else
		{
			source.mode = VisualizationMode::Color;
		}
		debugSources.push_back(source);
		debugSourceItems.push_back(target->GetName());
	}

	const std::vector<DepthBuffer*>& depthBuffers = renderContext.GetDepthBuffers();
	for (size_t i = 0; i < depthBuffers.size(); ++i)
	{
		DebugSource source;
		source.texture = renderContext.GetTexture(HDepthBuffer(i));
		source.mode = VisualizationMode::Depth;
		debugSources.push_back(source);
		debugSourceItems.push_back(depthBuffers[i]->GetName());
	}

	settings.AddCombo(
		GetName(),
		"blit_source",
		"Source",
		&debugSourceIndex,
		debugSourceItems.data(),
		static_cast<int>(debugSourceItems.size()));

	settings.AddText(
		GetName(),
		"blit_source_info",
		"Source Info",
		sourceInfo);
}

HTexture BlitPass::GetDefaultSourceTexture() const
{
	return
#if IS_EDITOR
		renderContext.GetTexture("RT_ImGui");
#else
		renderContext.GetTexture("CompositionTexture");
#endif
}

void BlitPass::Execute()
{
	auto frameIndex = deviceContext.GetCurrentBackBufferIndex();
	HTexture backBuffer = HTexture(frameIndex);

	const bool overrideActive = debugSourceIndex > 0
		&& debugSourceIndex <= static_cast<int>(debugSources.size());

	HTexture sourceTexture;
	if (overrideActive)
	{
		// Debug view: decode/letterbox the picked texture into the intermediate,
		// then copy that to the back buffer (same size and format by construction).
		const DebugSource& source = debugSources[debugSourceIndex - 1];
		int mode = source.mode;

		renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
		renderContext.SetDescriptorHeapCompute(commandList);
		renderContext.SetInlineConstantsUAV(commandList, 1, &mode, 0);

		renderContext.TransitionTo(commandList, debugTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
		renderContext.TransitionTo(commandList, source.texture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
		renderContext.BindTextureOnlyUAV(commandList, debugTexture, 1);
		// The shader declares a float view at t0 and a uint view at t1 and only
		// reads the one matching the mode, so bind the source to both slots.
		renderContext.BindTextureOnlySRV(commandList, source.texture, 2);
		renderContext.BindTextureOnlySRV(commandList, source.texture, 3);
		renderContext.Dispatch(commandList, (1920 + 7) / 8, (1080 + 7) / 8, 1);
		renderContext.TransitionBack(commandList, source.texture);

#if IS_EDITOR
		// Keep the UI usable: ImGuiPass shows DebugBlitTexture in its Viewport
		// window instead, and the normal UI still reaches the screen.
		sourceTexture = GetDefaultSourceTexture();
#else
		sourceTexture = debugTexture;
#endif
	}
	else
	{
		sourceTexture = GetDefaultSourceTexture();
	}

	// ImGuiPass runs before this pass, so it picks the change up next frame.
	renderContext.SetDebugViewActive(overrideActive);

	renderContext.TransitionTo(commandList, sourceTexture, D3D12_RESOURCE_STATE_COPY_SOURCE);
	renderContext.TransitionTo(commandList, backBuffer, D3D12_RESOURCE_STATE_COPY_DEST);
	// DebugBlitTexture is a fixed 1920x1080 regardless of the actual back
	// buffer size, so clamp the copy region instead of assuming a match.
	renderContext.CopyTextureClamped(commandList, sourceTexture, backBuffer);

	renderContext.TransitionBack(commandList, sourceTexture);
	renderContext.TransitionTo(commandList, backBuffer, D3D12_RESOURCE_STATE_PRESENT);
}

void BlitPass::PostSubmit()
{
}

void BlitPass::Allocate(DeviceContext* deviceContext)
{
}
