#include "TestPass.h"

#include "RenderContext.h"
#include "DeviceContext.h"

extern DeviceContext deviceContext;
extern RenderContext renderContext;

TestPass::TestPass()
{
}

TestPass::~TestPass()
{
}

void TestPass::Prepare()
{
	renderTargetIndex = renderContext.CreateRenderTarget();
	renderContext.CreateVertexBuffer(&deviceContext);
	deviceContext.Flush();
}

void TestPass::Execute()
{
	renderContext.ResetCommandList(commandListIndex);
	renderContext.SetupRenderPass(commandListIndex, rootSignatureIndex, viewportAndScissorsIndex, viewportAndScissorsIndex);
	renderContext.BindRenderTarget(commandListIndex, renderTargetIndex);
	renderContext.CleraRenderTarget(commandListIndex, renderTargetIndex);
	renderContext.BindGeometry(commandListIndex);

	float cameraMatrix[16] = {
		1.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
	};
	renderContext.SetInlineConstants(commandListIndex, 16, cameraMatrix);

	auto commandList = renderContext.GetCommandList(commandListIndex);
	commandList->GetCommandList()->DrawInstanced(36, 1, 0, 0);

	renderContext.CloseCommandList(commandListIndex);
	renderContext.ExecuteCommandList(commandListIndex);
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}