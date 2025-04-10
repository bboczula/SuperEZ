#include "TestPass.h"

#include "RenderContext.h"
#include "DeviceContext.h"
#include "WindowContext.h"
#include "InputLayout.h"
#include "camera/PerspectiveCamera.h"
#include "camera/OrthographicCamera.h"
#include "camera/Arcball.h"

#include <pix3.h>

extern DeviceContext deviceContext;
extern RenderContext renderContext;
extern WindowContext windowContext;

#define USE_PERSPECTIVE_CAMERA 1

TestPass::TestPass() : RenderPass(L"Test", Type::Default)
{
	const auto aspectRatio = static_cast<float>(windowContext.GetWidth()) / static_cast<float>(windowContext.GetHeight());
#if USE_PERSPECTIVE_CAMERA
	perspectiveCamera = new PerspectiveCamera(aspectRatio, DirectX::SimpleMath::Vector3(0.0f, 1.0f, 2.0f));
	arcballCamera = new Arcball(perspectiveCamera);
#else
	float distanceToPlane = 2.0f;
	float fov = DirectX::XMConvertToRadians(36.0f);
	float height = tan(fov * 0.5f) * distanceToPlane;
	float width = height * aspectRatio;
	width *= 2.0f;
	height *= 2.0f;

	orthoCamera = new OrthographicCamera(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 2.0f));
	orthoCamera->SetWidth(width);
	orthoCamera->SetHeight(height);
	arcballCamera = new Arcball(orthoCamera);
#endif
}

TestPass::~TestPass()
{
#if USE_PERSPECTIVE_CAMERA
	delete perspectiveCamera;
#else
	delete orthoCamera;
#endif
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
	arcballCamera->Rotate(0.0f, 0.25f, 0.0f);
}

void TestPass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature, viewportAndScissors);
	renderContext.BindRenderTargetWithDepth(commandList, renderTarget, depthBuffer);
	renderContext.CleraRenderTarget(commandList, renderTarget);
	renderContext.ClearDepthBuffer(commandList, depthBuffer);

#if USE_PERSPECTIVE_CAMERA
	renderContext.SetInlineConstants(commandList, 16, perspectiveCamera->GetViewProjectionMatrixPtr());
#else
	renderContext.SetInlineConstants(commandList, 16, orthoCamera->GetViewProjectionMatrixPtr());
#endif

	for (int i = 0; i < 33; i++)
	{
		renderContext.BindGeometry(commandList, HMesh(i));
		renderContext.DrawMesh(commandList, HMesh(i));
	}
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}