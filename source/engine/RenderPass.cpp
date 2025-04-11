#include "RenderPass.h"
#include "RenderContext.h"
#include "InputLayout.h"

#ifdef DEBUG
#define USE_PIX
#endif
#include "pix3.h"

#include <assert.h>

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderPass::RenderPass(PCWSTR name, Type type) : shaderSourceFileName(L"shaders.hlsl"), name(name), type(type)
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::AutomaticPrepare()
{
	if (GetType() == Type::Default)
	{
		rootSignature = renderContext.CreateRootSignature(&deviceContext);
		shader = renderContext.CreateShaders(shaderSourceFileName);
		pipelineState = renderContext.CreatePipelineState(&deviceContext, rootSignature, shader, inputLayout);
		viewportAndScissors = renderContext.CreateViewportAndScissorRect(&deviceContext);
	}

	commandList = renderContext.CreateCommandList();
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
	if (GetType() == Type::Default)
	{
		renderContext.ResetCommandList(commandList, pipelineState);
	}
	else if (GetType() == Type::Drawless)
	{
		renderContext.ResetCommandList(commandList);
	}
	PIXBeginEvent(renderContext.GetCommandList(commandList)->GetCommandList(), 0, name);
}

void RenderPass::PostExecute()
{
	PIXEndEvent(renderContext.GetCommandList(commandList)->GetCommandList());
	renderContext.CloseCommandList(commandList);
	renderContext.ExecuteCommandList(commandList);
}
