#include "ForwardPass.h"

#include "../RenderContext.h"
#include "../../core/DeviceContext.h"
#include "../../engine/WindowContext.h"
#include "../../core/InputLayout.h"
#include "../../engine/camera/Camera.h"
#include "../../engine/camera/Orbit.h"
#include "../../engine/camera/Free.h"
#include "../../engine/input/RawInput.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../externals/SimpleMath/SimpleMath.cpp"

#include <pix3.h>

extern DeviceContext deviceContext;
extern RenderContext renderContext;
extern WindowContext windowContext;
extern RawInput rawInput;

ForwardPass::ForwardPass() : RenderPass(L"Forward", L"shaders.hlsl", Type::Graphics)
{
}

void ForwardPass::SetOrthographicProperties(const float aspectRatio)
{
	float distanceToPlane = 5.0f;
	float fov = DirectX::XMConvertToRadians(36.0f);
	float height = tan(fov * 0.5f) * distanceToPlane;
	float width = height * aspectRatio;
	width *= 2.0f;
	height *= 2.0f;

	renderContext.GetActiveCamera()->SetWidth(width);
	renderContext.GetActiveCamera()->SetHeight(height);
}

void ForwardPass::PostAssetLoad()
{
	// Create camera (can't be done in constructor because renderContext is not ready)
	freeCamera = new FreeCamera(renderContext.GetActiveCamera());
}

ForwardPass::~ForwardPass()
{
}

void ForwardPass::ConfigurePipelineState()
{
	// Pre-AutomaticInitialize Procedure
	inputLayout = renderContext.CreateInputLayout();
	renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position, VertexStream::Color, VertexStream::TexCoord, VertexStream::Normal);

	// Now we can create the root signature
	RootSignatureBuilder builder;
	builder.AddConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddConstants(16, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddCBV(2, D3D12_SHADER_VISIBILITY_PIXEL); // CBV t2 (light data)
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // SRV t0
	builder.AddSamplerTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // Sampler s0
	builder.AddSRVTable(1, 1, D3D12_SHADER_VISIBILITY_PIXEL); // SRV t1 (shadow map)
	builder.AddCBV(3, D3D12_SHADER_VISIBILITY_VERTEX); // CBV b3 (light view-projection)
	builder.AddCBV(4, D3D12_SHADER_VISIBILITY_PIXEL); // CBV b4 (debug settings)
	builder.AddConstants(4, 5, 0, D3D12_SHADER_VISIBILITY_PIXEL); // Root Constants @ b5 (camera position)
	builder.AddConstants(4, 6, 0, D3D12_SHADER_VISIBILITY_PIXEL); // Root Constants @ b6 (material)
	rootSignature = renderContext.CreateRootSignature(builder);

	// Menu height seems to be 20 pixels
	// The actual viewport size is this ImVec2(1920 - 400, 1080 - menuHeight - 25);
	int viewportWidth = 1920;
	int viewportHeight = 1080;
#if IS_EDITOR
	const int menuHeight = 20;
	viewportWidth -= 400; // Assuming the menu takes 400 pixels
	viewportHeight -= menuHeight - 25; // Assuming the status bar takes 25 pixels
#endif
	renderTarget = renderContext.CreateRenderTarget("RT_ForwardPass", RenderTargetFormat::RGB8_UNORM, viewportWidth, viewportHeight);
	depthBuffer = renderContext.CreateDepthBuffer();
	sunlightBuffer = renderContext.CreateConsantBuffer<SunlightConstants>();
	const SunlightConstants& sunlightConstants = renderContext.GetSunlightConstants();
	renderContext.UpdateConstantBuffer(sunlightBuffer, &sunlightConstants, sizeof(sunlightConstants));

	debugSettingsBuffer = renderContext.CreateConsantBuffer<DebugSettings>();
	DebugSettings debugSettings;
	debugSettings.forceMipLevel = 0.0f;
	renderContext.UpdateConstantBuffer(debugSettingsBuffer, &debugSettings, sizeof(debugSettings));

	sunlightViewProjectionBuffer = renderContext.CreateConsantBuffer<SunlightViewProjection>();
	const SunlightViewProjection& sunlightViewProjection = renderContext.GetSunlightViewProjection();
	renderContext.UpdateConstantBuffer(sunlightViewProjectionBuffer, &sunlightViewProjection, sizeof(sunlightViewProjection));
	deviceContext.Flush();
}

void ForwardPass::Initialize()
{
}

void ForwardPass::Update()
{
	renderContext.SetLinearColorEnabled(useLinearColor);

	if (rawInput.IsKeyDown(VK_NUMPAD1) || rawInput.IsKeyDown(VK_NUMPAD3) || rawInput.IsKeyDown(VK_NUMPAD7))
	{
		// There is a crash, somehow we keep entering this condition, even though we don't press any key
		renderContext.GetActiveCamera()->SetRotation(DirectX::SimpleMath::Vector3(.0f, 0.0f, 0.0f));
		renderContext.GetActiveCamera()->SetPosition(DirectX::SimpleMath::Vector3(
			2.0f * rawInput.IsKeyDown(VK_NUMPAD1),
			2.0f * rawInput.IsKeyDown(VK_NUMPAD7),
			2.0f * rawInput.IsKeyDown(VK_NUMPAD3) + 0.0000001));
	}
	else if (rawInput.WasKeyDown(VK_NUMPAD5))
	{
		isPerspectiveCamera = !isPerspectiveCamera;
		if (!isPerspectiveCamera)
		{
			SetOrthographicProperties(static_cast<float>(windowContext.GetWidth()) / static_cast<float>(windowContext.GetHeight()));
		}
	}

	freeCamera->SetCamera(renderContext.GetActiveCamera());

	if (rawInput.IsRightButtonDown())
	{		
		float dx = rawInput.GetMouseXDelta();
		float dy = rawInput.GetMouseYDelta();
		float sensitivity = 0.1f;
		freeCamera->Rotate(dy * sensitivity, -dx * sensitivity, 0.0f); // match your sign convention
	}

	// Scale movement to the loaded scene so large imported scenes stay navigable
	const float sceneRadius = renderContext.GetSceneBoundsRadius();
	float cameraSpeed = max(0.01f, sceneRadius * 0.0015f);
	if (rawInput.IsKeyDown(VK_SHIFT))
	{
		cameraSpeed *= 8.0f;
	}
	if(rawInput.IsKeyDown('W'))
	{
		freeCamera->MoveForward(cameraSpeed);
	}
	if(rawInput.IsKeyDown('S'))
	{
		freeCamera->MoveBackward(cameraSpeed);
	}
	if(rawInput.IsKeyDown('D'))
	{
		freeCamera->MoveLeft(cameraSpeed);
	}
	if(rawInput.IsKeyDown('A'))
	{
		freeCamera->MoveRight(cameraSpeed);
	}
}

void ForwardPass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeap(commandList);
	renderContext.BindRenderTargetWithDepth(commandList, renderTarget, depthBuffer);
	renderContext.CleraRenderTarget(commandList, renderTarget);
	renderContext.ClearDepthBuffer(commandList, depthBuffer);

	auto type = isPerspectiveCamera ? Camera::CameraType::PERSPECTIVE : Camera::CameraType::ORTHOGRAPHIC;
	renderContext.GetActiveCamera()->SetType(type);
	renderContext.SetInlineConstants(commandList, renderContext.GetActiveCamera()->ViewProjecttion(), 0);
	const DirectX::SimpleMath::Vector3 cameraPosition = renderContext.GetActiveCamera()->GetPosition();
	const DirectX::SimpleMath::Vector4 cameraPositionConstants(cameraPosition.x, cameraPosition.y, cameraPosition.z, 1.0f);
	renderContext.SetInlineConstants(commandList, cameraPositionConstants, 8);
	const SunlightConstants& sunlightConstants = renderContext.GetSunlightConstants();
	renderContext.UpdateConstantBuffer(sunlightBuffer, &sunlightConstants, sizeof(sunlightConstants));
	renderContext.BindConstantBuffer(commandList, sunlightBuffer, 2);
	const SunlightViewProjection& sunlightViewProjection = renderContext.GetSunlightViewProjection();
	renderContext.UpdateConstantBuffer(sunlightViewProjectionBuffer, &sunlightViewProjection, sizeof(sunlightViewProjection));
	renderContext.BindConstantBuffer(commandList, sunlightViewProjectionBuffer, 6);
	debugSettings.visualizeSelectedMip = visualizeSelectedMip ? 1 : 0;
	renderContext.UpdateConstantBuffer(debugSettingsBuffer, &debugSettings, sizeof(debugSettings));
	renderContext.BindConstantBuffer(commandList, debugSettingsBuffer, 7);
	HTexture shadowMapTexture = renderContext.GetShadowMapTexture();
	if (shadowMapTexture.IsValid())
	{
		renderContext.BindTextureSRV(commandList, shadowMapTexture, 5);
	}

	const auto& items = renderContext.GetRenderItems();
	for (const RenderItem& item : items)
	{
		const DirectX::SimpleMath::Vector4 materialConstants(
			item.diffuseStrength,
			item.specularStrength,
			item.shininess,
			0.0f);
		renderContext.SetInlineConstants(commandList, item.World(), 1);
		renderContext.SetInlineConstants(commandList, materialConstants, 9);
		renderContext.BindGeometry(commandList, item.mesh);
		renderContext.BindTexture(commandList, item.texture, 3);
		renderContext.DrawMesh(commandList, item.mesh);
	}
}

void ForwardPass::PostSubmit()
{
}

void ForwardPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}

void ForwardPass::RegisterSettings(RenderPassSettings& settings)
{
	static const char* const mipModeItems[] =
	{
		"Implicit",
		"Biased",
		"Forced Level"
	};
	static const char* const mipLegendLabels[] =
	{
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7+"
	};
	static const unsigned int mipLegendColors[] =
	{
		0xff0000ff,
		0xff8000ff,
		0xffff00ff,
		0x00ff00ff,
		0x00ffffff,
		0x0040ffff,
		0xbf00ffff,
		0xff00ffff
	};

#if ENABLE_COLOR_PIPELINE_DEBUG
	settings.AddBool(
		GetName(),
		"use_linear_color",
		"Use Linear Color",
		&useLinearColor);
#endif

	settings.AddCombo(
		GetName(),
		"mip_mode",
		"Mip Mode",
		&debugSettings.mipMode,
		mipModeItems,
		static_cast<int>(sizeof(mipModeItems) / sizeof(mipModeItems[0])));

	settings.AddFloat(
		GetName(),
		"force_mip_level",
		"Force Mip Level",
		&debugSettings.forceMipLevel,
		0.0f,
		10.0f,
		1.0f);

	settings.AddFloat(
		GetName(),
		"shader_mip_bias",
		"Shader Mip Bias",
		&debugSettings.shaderMipBias,
		-10.0f,
		10.0f,
		0.1f);

	settings.AddBool(
		GetName(),
		"visualize_selected_mip",
		"Visualize selected mip",
		&visualizeSelectedMip);

	settings.AddColorLegend(
		GetName(),
		"mip_color_scale",
		"Mip Color Scale",
		mipLegendLabels,
		mipLegendColors,
		static_cast<int>(sizeof(mipLegendLabels) / sizeof(mipLegendLabels[0])));

	settings.AddFloat(
		GetName(),
		"mip_visualization_strength",
		"Mip Visualization Strength",
		&debugSettings.mipVisualizationStrength,
		0.0f,
		1.0f,
		0.01f);
}
