#include "TestPass.h"

#include "../RenderContext.h"
#include "../../core/DeviceContext.h"
#include "../../engine/WindowContext.h"
#include "../../core/InputLayout.h"
#include "../../engine/camera/Camera.h"
#include "../../engine/camera/Orbit.h"
#include "../../engine/input/RawInput.h"
#include "../RootSignatureBuilder.h"

#include <pix3.h>

extern DeviceContext deviceContext;
extern RenderContext renderContext;
extern WindowContext windowContext;
extern RawInput rawInput;

TestPass::TestPass() : RenderPass(L"Test", L"shaders.hlsl", Type::Default)
{
	arcballCamera = new Orbit(renderContext.GetCamera(0));
}

void TestPass::SetOrthographicProperties(const float aspectRatio)
{
	float distanceToPlane = arcballCamera->GetRadius();
	float fov = DirectX::XMConvertToRadians(36.0f);
	float height = tan(fov * 0.5f) * distanceToPlane;
	float width = height * aspectRatio;
	width *= 2.0f;
	height *= 2.0f;

	renderContext.GetCamera(0)->SetWidth(width);
	renderContext.GetCamera(0)->SetHeight(height);
}

TestPass::~TestPass()
{
}

void TestPass::ConfigurePipelineState()
{
	// Pre-AutomaticInitialize Procedure
	inputLayout = renderContext.CreateInputLayout();
	renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position, VertexStream::Color, VertexStream::TexCoord);

	// Now we can create the root signature
	RootSignatureBuilder builder;
	builder.AddConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // SRV t0
	builder.AddSamplerTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // Sampler s0
	rootSignature = renderContext.CreateRootSignature(builder);

	// Menu height seems to be 20 pixels
	// The actual viewport size is this ImVec2(1920 - 400, 1080 - menuHeight - 25);
	const int menuHeight = 20;
	const int viewportWidth = 1920 - 400; // Assuming the menu takes 400 pixels
	const int viewportHeight = 1080 - menuHeight - 25; // Assuming the status bar takes 25 pixels
	renderTarget = renderContext.CreateRenderTarget("RT_TestPass", RenderTargetFormat::RGB8_UNORM, viewportWidth, viewportHeight);
	depthBuffer = renderContext.CreateDepthBuffer();
	deviceContext.Flush();
}

void TestPass::Initialize()
{
}

void TestPass::Update()
{
	const bool isXAxisRotation = rawInput.GetMouseXDelta() != 0 && rawInput.IsMiddleButtonDown();
	const bool isYAxisRotation = rawInput.GetMouseYDelta() != 0 && rawInput.IsMiddleButtonDown();
	if (isXAxisRotation || isYAxisRotation)
	{
		arcballCamera->Rotate(0.1f * rawInput.GetMouseYDelta(), -0.1f * rawInput.GetMouseXDelta(), 0.0f);
	}

	const bool hasMouseWheelMoved = rawInput.HasMouseWheelMoved();
	if (hasMouseWheelMoved)
	{
		SHORT wheelDelta = rawInput.GetMouseWheelDelta();
		if (wheelDelta != 0.0f)
		{
			float zoomStep = 0.1f * static_cast<float>(wheelDelta) / 120.0f;
			arcballCamera->SetRadius(arcballCamera->GetRadius() + zoomStep);
		}
	}

	if (rawInput.IsKeyDown(VK_NUMPAD1) || rawInput.IsKeyDown(VK_NUMPAD3) || rawInput.IsKeyDown(VK_NUMPAD7))
	{
		// There is a crash, somehow we keep entering this condition, even though we don't press any key
		auto radius = arcballCamera->GetRadius();
		renderContext.GetCamera(0)->SetRotation(DirectX::SimpleMath::Vector3(.0f, 0.0f, 0.0f));
		renderContext.GetCamera(0)->SetPosition(DirectX::SimpleMath::Vector3(
			2.0f * rawInput.IsKeyDown(VK_NUMPAD1),
			2.0f * rawInput.IsKeyDown(VK_NUMPAD7),
			2.0f * rawInput.IsKeyDown(VK_NUMPAD3) + 0.0000001));
		arcballCamera->SetRadius(radius);
		arcballCamera->RecalculateBasisVectors();
	}
	else if (rawInput.WasKeyDown(VK_NUMPAD5))
	{
		isPerspectiveCamera = !isPerspectiveCamera;
		if (!isPerspectiveCamera)
		{
			SetOrthographicProperties(static_cast<float>(windowContext.GetWidth()) / static_cast<float>(windowContext.GetHeight()));
		}
	}
}

void TestPass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature, viewportAndScissors);
	renderContext.SetDescriptorHeap(commandList);
	renderContext.BindRenderTargetWithDepth(commandList, renderTarget, depthBuffer);
	renderContext.CleraRenderTarget(commandList, renderTarget);
	renderContext.ClearDepthBuffer(commandList, depthBuffer);

	auto type = isPerspectiveCamera ? Camera::CameraType::PERSPECTIVE : Camera::CameraType::ORTHOGRAPHIC;
	renderContext.GetCamera(0)->SetType(type);
	renderContext.SetInlineConstants(commandList, 16, renderContext.GetCamera(0)->GetViewProjectionMatrixPtr(), 0);
	
	for (int i = 0; i < renderContext.GetNumOfMeshes(); i++)
	{
		renderContext.BindGeometry(commandList, HMesh(i));
		renderContext.BindTexture(commandList, HTexture(i), 1);
		renderContext.DrawMesh(commandList, HMesh(i));
	}
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}