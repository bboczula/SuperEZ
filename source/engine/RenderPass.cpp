#include "RenderPass.h"
#include "RenderContext.h"
#include "pix.h"

#include <assert.h>

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderPass::RenderPass() : shaderSourceFileName(L"shaders.hlsl")
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
