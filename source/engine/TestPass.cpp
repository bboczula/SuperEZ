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
	renderContext.CreateRootSignature(&deviceContext);
	shaderIndex = renderContext.CreateShaders(&deviceContext);
	renderContext.CreatePipelineState(&deviceContext);
	renderContext.CreateViewportAndScissorRect(&deviceContext);
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