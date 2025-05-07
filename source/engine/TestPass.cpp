#include "TestPass.h"

#include "RenderContext.h"
#include "DeviceContext.h"
#include "WindowContext.h"
#include "InputLayout.h"
#include "camera/Camera.h"
#include "camera/Orbit.h"
#include "input/RawInput.h"

#include <pix3.h>

extern DeviceContext deviceContext;
extern RenderContext renderContext;
extern WindowContext windowContext;
extern RawInput rawInput;

TestPass::TestPass() : RenderPass(L"Test", Type::Default)
{
	const auto aspectRatio = static_cast<float>(windowContext.GetWidth()) / static_cast<float>(windowContext.GetHeight());
	camera = new Camera(aspectRatio, DirectX::SimpleMath::Vector3(0.0f, 1.0f, 2.0f));
	arcballCamera = new Orbit(camera);
}

void TestPass::SetOrthographicProperties(const float aspectRatio)
{
	float distanceToPlane = arcballCamera->GetRadius();
	float fov = DirectX::XMConvertToRadians(36.0f);
	float height = tan(fov * 0.5f) * distanceToPlane;
	float width = height * aspectRatio;
	width *= 2.0f;
	height *= 2.0f;

	camera->SetWidth(width);
	camera->SetHeight(height);
}

TestPass::~TestPass()
{
	delete camera;
}

void TestPass::ConfigurePipelineState()
{
	// Pre-AutomaticPrepare Procedure
	inputLayout = renderContext.CreateInputLayout();
	renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position, VertexStream::Color);
}

void TestPass::Prepare()
{
	renderTarget = renderContext.CreateRenderTarget();
	depthBuffer = renderContext.CreateDepthBuffer();
	deviceContext.Flush();
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
		camera->SetRotation(DirectX::SimpleMath::Vector3(.0f, 0.0f, 0.0f));
		camera->SetPosition(DirectX::SimpleMath::Vector3(
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
	renderContext.BindRenderTargetWithDepth(commandList, renderTarget, depthBuffer);
	renderContext.CleraRenderTarget(commandList, renderTarget);
	renderContext.ClearDepthBuffer(commandList, depthBuffer);

	auto type = isPerspectiveCamera ? Camera::CameraType::PERSPECTIVE : Camera::CameraType::ORTHOGRAPHIC;
	renderContext.SetInlineConstants(commandList, 16, camera->GetViewProjectionMatrixPtr(type));
	
	for (int i = 0; i < renderContext.GetNumOfMeshes(); i++)
	{
		renderContext.BindGeometry(commandList, HMesh(i));
		renderContext.DrawMesh(commandList, HMesh(i));
	}
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}