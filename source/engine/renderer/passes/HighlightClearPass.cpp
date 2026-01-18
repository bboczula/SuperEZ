#include "HighlightClearPass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"

extern RenderContext renderContext;

HighlightClearPass::HighlightClearPass() : RenderPass(L"HighlightClear", L"highlight_clear.hlsl", Type::Compute)
{
}

HighlightClearPass::~HighlightClearPass()
{
}

void HighlightClearPass::ConfigurePipelineState()
{
    // Now we can create the root signature
    RootSignatureBuilder builder;
    builder.AddConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
    builder.AddConstants(1, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b1
    builder.AddUAVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // UAV u0
    rootSignature = renderContext.CreateRootSignature(builder);

    outputTexture = renderContext.CreateEmptyTexture(1920, 1080, DXGI_FORMAT_R32G32_UINT, "HighlightClearTexture", true);
}

void HighlightClearPass::Initialize()
{
}

void HighlightClearPass::Update()
{
}

void HighlightClearPass::Execute()
{
	// The input texture needs to be 4, previous ones don't have valid SRV offset
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeapCompute(commandList);
	unsigned int width = 1920;
	renderContext.SetInlineConstantsUAV(commandList, 1, &width, 0); // INVALID value
	unsigned int height = 1080;
	renderContext.SetInlineConstantsUAV(commandList, 1, &height, 1); // INVALID value
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 2); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture); // Input Texture
}

void HighlightClearPass::Allocate(DeviceContext* deviceContext)
{
}
