#include "SelectionPass.h"
#include "RenderContext.h"
#include "InputLayout.h"

extern RenderContext renderContext;

SelectionPass::SelectionPass() : RenderPass(L"Selection", L"selection.hlsl", Type::Default)
{
}

void SelectionPass::ConfigurePipelineState()
{
	// Pre-AutomaticInitialize Procedure
	inputLayout = renderContext.CreateInputLayout();
	renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);
}

void SelectionPass::Initialize()
{
}

void SelectionPass::Update()
{
}

void SelectionPass::Execute()
{
}

void SelectionPass::Allocate(DeviceContext* deviceContext)
{
}
