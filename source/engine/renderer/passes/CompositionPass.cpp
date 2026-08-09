#include "CompositionPass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"

extern RenderContext renderContext;

CompositionPass::CompositionPass() : RenderPass(L"Composition", L"composition.hlsl", Type::Compute)
{
}

CompositionPass::~CompositionPass()
{
}

void CompositionPass::ConfigurePipelineState()
{
	RootSignatureBuilder builder;
	builder.AddConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddConstants(1, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b1
	builder.AddConstants(1, 2, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b2
	builder.AddUAVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // UAV u0
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // SRV t0
#if IS_EDITOR
	builder.AddSRVTable(1, 1, D3D12_SHADER_VISIBILITY_ALL); // SRV t1
#endif
	rootSignature = renderContext.CreateRootSignature(builder);

	int viewportWidth = 1920;
	int viewportHeight = 1080;
#if IS_EDITOR
	const int menuHeight = 20;
	viewportWidth -= 400; // Assuming the menu takes 400 pixels
	viewportHeight -= menuHeight - 25; // Assuming the status bar takes 25 pixels
#endif
	TextureCreateDesc textureDesc;
	textureDesc.width = viewportWidth;
	textureDesc.height = viewportHeight;
	textureDesc.format = DXGI_FORMAT_R8G8B8A8_UNORM;
	textureDesc.name = "CompositionTexture";
	textureDesc.createUav = true;
	textureDesc.createRtv = true;
	textureDesc.staticSrv = true;
	textureDesc.staticUav = true;
	textureDesc.lifeSpan = APP;
	outputTexture = renderContext.CreateEmptyTexture(textureDesc);
}

void CompositionPass::PostAssetLoad()
{
}

void CompositionPass::Initialize()
{
}

void CompositionPass::Update()
{
}

void CompositionPass::Execute()
{
	HTexture sceneColorTexture = renderContext.GetTexture("RT_ForwardPass");
#if IS_EDITOR
	HTexture highlightTexture = renderContext.GetTexture("HighlightOutputTexture");
#endif
	// The input texture needs to be 4, previous ones don't have valid SRV offset
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeapCompute(commandList);
	const D3D12_RESOURCE_DESC outputDesc = renderContext.GetTexture(outputTexture)->GetResource()->GetDesc();
	unsigned int width = static_cast<unsigned int>(outputDesc.Width);
	renderContext.SetInlineConstantsUAV(commandList, 1, &width, 0);
	unsigned int height = outputDesc.Height;
	renderContext.SetInlineConstantsUAV(commandList, 1, &height, 1);
	unsigned int useLinearColor = renderContext.IsLinearColorEnabled() ? 1u : 0u;
	renderContext.SetInlineConstantsUAV(commandList, 1, &useLinearColor, 2);

	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, sceneColorTexture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
#if IS_EDITOR
	renderContext.TransitionTo(commandList, highlightTexture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE);
#endif
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 3); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, sceneColorTexture, 4);
#if IS_EDITOR
	renderContext.BindTextureOnlySRV(commandList, highlightTexture, 5);
#endif
	renderContext.Dispatch(commandList, (width + 7) / 8, (height + 7) / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture);
	renderContext.TransitionBack(commandList, sceneColorTexture);
#if IS_EDITOR
	renderContext.TransitionBack(commandList, highlightTexture);
#endif
}

void CompositionPass::PostSubmit()
{
}

void CompositionPass::Allocate(DeviceContext* deviceContext)
{
}
