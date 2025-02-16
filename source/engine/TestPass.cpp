#include "TestPass.h"

#include "RenderContext.h"
#include "DeviceContext.h"

#include <pix3.h>

extern DeviceContext deviceContext;
extern RenderContext renderContext;

TestPass::TestPass()
{
	camera = new PerspectiveCamera(1.0f, DirectX::SimpleMath::Vector3(0.0f, 0.0f, 2.0f));
	camera->SetPosition(DirectX::SimpleMath::Vector3(0.0f, 0.0f, 1.0f));
	arcballCamera = new Arcball(camera);
}

TestPass::~TestPass()
{
	delete camera;
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
	renderContext.ResetCommandList(commandListIndex);
	PIXBeginEvent(renderContext.GetCommandList(commandListIndex)->GetCommandList(), 0, L"Test Pass");
	renderContext.SetupRenderPass(commandListIndex, pipelineStateIndex, rootSignatureIndex, viewportAndScissorsIndex, viewportAndScissorsIndex);
	renderContext.BindRenderTargetWithDepth(commandListIndex, renderTargetIndex, depthBufferIndex);
	renderContext.CleraRenderTarget(commandListIndex, renderTargetIndex);
	renderContext.ClearDepthBuffer(commandListIndex, depthBufferIndex);
	renderContext.BindGeometry(commandListIndex);

	renderContext.SetInlineConstants(commandListIndex, 16, camera->GetViewProjectionMatrixPtr());

	auto commandList = renderContext.GetCommandList(commandListIndex);
	commandList->GetCommandList()->DrawInstanced(36, 1, 0, 0);
	PIXEndEvent(renderContext.GetCommandList(commandListIndex)->GetCommandList());

	renderContext.CloseCommandList(commandListIndex);
	renderContext.ExecuteCommandList(commandListIndex);
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}