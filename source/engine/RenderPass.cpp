#include "RenderPass.h"
#include "RenderContext.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderPass::RenderPass()
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::AutomaticPrepare()
{
	rootSignatureIndex = renderContext.CreateRootSignature(&deviceContext);
	shaderIndex = renderContext.CreateShaders(&deviceContext);
	pipelineStateIndex = renderContext.CreatePipelineState(&deviceContext, rootSignatureIndex, shaderIndex);
	viewportAndScissorsIndex = renderContext.CreateViewportAndScissorRect(&deviceContext);
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
