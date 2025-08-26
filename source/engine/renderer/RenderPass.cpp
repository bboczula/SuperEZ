#include "RenderPass.h"
#include "RenderContext.h"
#include "../core/InputLayout.h"

#ifdef DEBUG
#define USE_PIX
#endif
#include "pix3.h"

#include <assert.h>

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderPass::RenderPass(PCWSTR name, LPCWSTR shaderName, Type type) : shaderSourceFileName(shaderName), name(name), type(type)
{
}

RenderPass::~RenderPass()
{
}

void RenderPass::AutomaticInitialize()
{
	if (GetType() == Type::Default)
	{
		vertexShader = renderContext.CreateShader(shaderSourceFileName, "VSMain", "vs_5_0");
		pixelShader = renderContext.CreateShader(shaderSourceFileName, "PSMain", "ps_5_0");
		pipelineState = renderContext.CreatePipelineState(&deviceContext, rootSignature, vertexShader, pixelShader, inputLayout, renderTarget);
		viewportAndScissors = renderContext.CreateViewportAndScissorRect(&deviceContext, renderTarget);
	}

	commandList = renderContext.CreateCommandList();
}

void RenderPass::Initialize()
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
