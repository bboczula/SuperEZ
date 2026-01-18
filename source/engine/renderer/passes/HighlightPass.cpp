#include "HighlightPass.h"
#include "../RenderContext.h"
#include "../../core/InputLayout.h"
#include "../../bind/RootSignatureBuilder.h"

extern RenderContext renderContext;

HighlightPass::HighlightPass() : RenderPass(L"Highlight", L"highlight.hlsl", Type::Compute)
{
}

void HighlightPass::ConfigurePipelineState()
{
	RootSignatureBuilder builder;
	builder.AddConstants(1, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddConstants(1, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b1
	builder.AddUAVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // UAV u0
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // SRV t0
	rootSignature = renderContext.CreateRootSignature(builder);

	const int menuHeight = 20;
	const int viewportWidth = 1920 - 400; // Assuming the menu takes 400 pixels
	const int viewportHeight = 1080 - menuHeight - 25; // Assuming the status bar takes 25 pixels
	outputTexture = renderContext.CreateEmptyTexture(viewportWidth, viewportHeight, DXGI_FORMAT_R32_UINT, "HighlightOutputTexture", true);
}

void HighlightPass::Initialize()
{
}

void HighlightPass::Update()
{
}

void HighlightPass::Execute()
{
	// The input texture needs to be 4, previous ones don't have valid SRV offset
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeapCompute(commandList);
	unsigned int width = 1920;
	renderContext.SetInlineConstantsUAV(commandList, 1, &width, 0); // INVALID value
	unsigned int height = 1080;
	renderContext.SetInlineConstantsUAV(commandList, 1, &height, 1); // INVALID value

	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, HTexture(6), 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture);
	renderContext.TransitionBack(commandList, HTexture(6)); // Input Texture

	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, HTexture(6), 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, outputTexture, 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, HTexture(6));
	renderContext.TransitionBack(commandList, outputTexture); // Input Texture

	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, HTexture(6), 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture);
	renderContext.TransitionBack(commandList, HTexture(6)); // Input Texture

	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, HTexture(6), 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, outputTexture, 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, HTexture(6));
	renderContext.TransitionBack(commandList, outputTexture); // Input Texture

	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, HTexture(6), 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture);
	renderContext.TransitionBack(commandList, HTexture(6)); // Input Texture

	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, HTexture(6), 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, outputTexture, 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, HTexture(6));
	renderContext.TransitionBack(commandList, outputTexture); // Input Texture

	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, HTexture(6), 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture);
	renderContext.TransitionBack(commandList, HTexture(6)); // Input Texture

	renderContext.TransitionTo(commandList, HTexture(6), D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, HTexture(6), 2); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, outputTexture, 3); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, HTexture(6));
	renderContext.TransitionBack(commandList, outputTexture); // Input Texture
}

void HighlightPass::Allocate(DeviceContext* deviceContext)
{
}
