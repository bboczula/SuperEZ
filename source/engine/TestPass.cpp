#include "TestPass.h"

#include "RenderContext.h"
#include "DeviceContext.h"
#include "camera/PerspectiveCamera.h"
#include "camera/OrthographicCamera.h"
#include "camera/Arcball.h"

#include <pix3.h>

extern DeviceContext deviceContext;
extern RenderContext renderContext;

#define USE_PERSPECTIVE_CAMERA 0

TestPass::TestPass() : RenderPass(L"Test", Type::Default)
{
#if USE_PERSPECTIVE_CAMERA
	perspectiveCamera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 2.0f));
	arcballCamera = new Arcball(perspectiveCamera);
#else
	orthoCamera = new OrthographicCamera(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 2.0f));
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
	renderContext.CreateVertexBuffer(&deviceContext);
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

	auto commandList = renderContext.GetCommandList(commandListIndex);
	commandList->GetCommandList()->DrawInstanced(36, 1, 0, 0);
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}