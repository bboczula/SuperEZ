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
	rootSignatureIndex = renderContext.CreateRootSignature(&deviceContext);
	shaderIndex = renderContext.CreateShaders(&deviceContext);
	pipelineStateIndex = renderContext.CreatePipelineState(&deviceContext, rootSignatureIndex, shaderIndex);
	viewportAndScissorsIndex = renderContext.CreateViewportAndScissorRect(&deviceContext);
	renderContext.CreateVertexBuffer(&deviceContext);
	deviceContext.Flush();
}

void TestPass::Execute()
{
	renderContext.PopulateCommandList(&deviceContext);
	renderContext.ExecuteCommandList(&deviceContext);
}

void TestPass::Allocate(DeviceContext* deviceContext)
{
	// We want our Render Pass
}