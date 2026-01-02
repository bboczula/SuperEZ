#include "HighlightPass.h"
#include "../RenderContext.h"
#include "../../core/InputLayout.h"
#include "../../bind/RootSignatureBuilder.h"

extern RenderContext renderContext;

HighlightPass::HighlightPass() : RenderPass(L"Highlight", L"highlight.hlsl", Type::Default)
{
}

void HighlightPass::ConfigurePipelineState()
{
	// This is a fullscreen pass
	inputLayout = renderContext.CreateInputLayout();
	renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);

	// For now, empty root signature is fine
	RootSignatureBuilder builder;
	rootSignature = renderContext.CreateRootSignature(builder);

	// I also need the Render Target
	renderTarget = renderContext.CreateRenderTarget("RT_HighlightPass", RenderTargetFormat::RGB8_UNORM, 1920, 1080);
}

void HighlightPass::Initialize()
{
}

void HighlightPass::Update()
{
}

void HighlightPass::Execute()
{
}

void HighlightPass::Allocate(DeviceContext* deviceContext)
{
}
