#include "RenderPass.h"
#include "RenderContext.h"

#ifdef DEBUG
#define USE_PIX
#endif
#include "pix3.h"

#include <assert.h>

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderPass::RenderPass(PCWSTR name) : shaderSourceFileName(L"shaders.hlsl"), name(name)
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::AutomaticPrepare()
{
	rootSignatureIndex = renderContext.CreateRootSignature(&deviceContext);
	shaderIndex = renderContext.CreateShaders(shaderSourceFileName);
	pipelineStateIndex = renderContext.CreatePipelineState(&deviceContext, rootSignatureIndex, shaderIndex);
	viewportAndScissorsIndex = renderContext.CreateViewportAndScissorRect(&deviceContext);

	commandListIndex = renderContext.CreateCommandList();
}

void RenderPass::Prepare()
{
	// Barrier management
	// Clearing render targets
	// Setting up render targets
	// Setting up shaders
	// Setting up vertex buffers
	// Resource bindings
}

void RenderPass::Update()
{
}

void RenderPass::PreExecute()
{
	renderContext.ResetCommandList(commandListIndex);
	PIXBeginEvent(renderContext.GetCommandList(commandListIndex)->GetCommandList(), 0, name);
}

void RenderPass::PostExecute()
{
	PIXEndEvent(renderContext.GetCommandList(commandListIndex)->GetCommandList());
	renderContext.CloseCommandList(commandListIndex);
	renderContext.ExecuteCommandList(commandListIndex);
}
