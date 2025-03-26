#include "TestPass.h"

#include "RenderContext.h"
#include "DeviceContext.h"
#include "camera/PerspectiveCamera.h"
#include "camera/OrthographicCamera.h"
#include "camera/Arcball.h"

#include <pix3.h>

extern DeviceContext deviceContext;
extern RenderContext renderContext;

#define USE_PERSPECTIVE_CAMERA 1

TestPass::TestPass() : RenderPass(L"Test", Type::Default)
{
#if USE_PERSPECTIVE_CAMERA
	perspectiveCamera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 2.0f));
	arcballCamera = new Arcball(perspectiveCamera);
#else
	float aspectRatio = 1.0f;
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

void TestPass::Prepare()
{
	renderTargetIndex = renderContext.CreateRenderTarget();
	depthBufferIndex = renderContext.CreateDepthBuffer();
	renderContext.CreateMesh();
	deviceContext.Flush();
}

void TestPass::Update()
{
	arcballCamera->Rotate(0.0f, 1.0f, 0.0f);
}

void TestPass::Execute()
{
	renderContext.SetupRenderPass(commandListIndex, pipelineStateIndex, rootSignatureIndex, viewportAndScissorsIndex, viewportAndScissorsIndex);
	renderContext.BindRenderTargetWithDepth(commandListIndex, renderTargetIndex, depthBufferIndex);
	renderContext.CleraRenderTarget(commandListIndex, renderTargetIndex);
	renderContext.ClearDepthBuffer(commandListIndex, depthBufferIndex);
	renderContext.BindGeometry(commandListIndex);

#if USE_PERSPECTIVE_CAMERA
	renderContext.SetInlineConstants(commandListIndex, 16, perspectiveCamera->GetViewProjectionMatrixPtr());
#else
	renderContext.SetInlineConstants(commandListIndex, 16, orthoCamera->GetViewProjectionMatrixPtr());
#endif

	renderContext.DrawMesh(commandListIndex, 0);
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}