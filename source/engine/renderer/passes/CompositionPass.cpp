#include "CompositionPass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"

extern RenderContext renderContext;

CompositionPass::CompositionPass() : RenderPass(L"Composition", L"composition.hlsl", Type::Compute)
{
}

CompositionPass::~CompositionPass()
{
}

void CompositionPass::ConfigurePipelineState()
{
	RootSignatureBuilder builder;
	builder.AddConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddConstants(1, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b1
	builder.AddUAVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // UAV u0
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // SRV t0
	builder.AddSRVTable(1, 1, D3D12_SHADER_VISIBILITY_ALL); // SRV t0
	rootSignature = renderContext.CreateRootSignature(builder);

	int viewportWidth = 1920;
	int viewportHeight = 1080;
#if IS_EDITOR
	const int menuHeight = 20;
	viewportWidth -= 400; // Assuming the menu takes 400 pixels
	viewportHeight -= menuHeight - 25; // Assuming the status bar takes 25 pixels
#endif
	outputTexture = renderContext.CreateEmptyTexture(viewportWidth, viewportHeight, DXGI_FORMAT_R8G8B8A8_UNORM, "CompositionTexture", true);
}

void CompositionPass::Initialize()
{
}

void CompositionPass::Update()
{
}

void CompositionPass::Execute()
{
	// The input texture needs to be 4, previous ones don't have valid SRV offset
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeapCompute(commandList);
	unsigned int width = 1920;
	renderContext.SetInlineConstantsUAV(commandList, 1, &width, 0); // INVALID value
	unsigned int height = 1080;
	renderContext.SetInlineConstantsUAV(commandList, 1, &height, 1); // INVALID value

	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, HTexture(4), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.TransitionTo(commandList, HTexture(8), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, HTexture(4), 3); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlySRV(commandList, HTexture(8), 4); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture);
	renderContext.TransitionBack(commandList, HTexture(4)); // Input Texture
	renderContext.TransitionBack(commandList, HTexture(8)); // Input Texture
}

void CompositionPass::Allocate(DeviceContext* deviceContext)
{
}
