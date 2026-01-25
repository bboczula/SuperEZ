#include "GrayscalePass.h"

#include "../RenderContext.h"
#include "../../core/DeviceContext.h"
#include "../../core/InputLayout.h"
#include "../../bind/RootSignatureBuilder.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

GrayscalePass::GrayscalePass() : RenderPass(L"Grayscale", L"grayscale.hlsl", Type::Compute)
{
}

GrayscalePass::~GrayscalePass()
{
}

void GrayscalePass::ConfigurePipelineState()
{
	RootSignatureBuilder builder;
	builder.AddUAVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // UAV u0
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_ALL); // SRV t0
	rootSignature = renderContext.CreateRootSignature(builder);

	outputTexture = renderContext.CreateEmptyTexture(1920, 1080, DXGI_FORMAT_R8G8B8A8_UNORM, "GrayscaleInputTexture", true);
}

void GrayscalePass::PostAssetLoad()
{
}

void GrayscalePass::Initialize()
{
}

void GrayscalePass::Update()
{
}

void GrayscalePass::Execute()
{
	// The input texture needs to be 4, previous ones don't have valid SRV offset
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeapCompute(commandList);
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_UNORDERED_ACCESS);
	renderContext.TransitionTo(commandList, HTexture(4), D3D12_RESOURCE_STATE_NON_PIXEL_SHADER_RESOURCE); // Input Texture (assumed to be at index 0)
	renderContext.BindTextureOnlyUAV(commandList, outputTexture, 0); // Output Texture
	renderContext.BindTextureOnlySRV(commandList, HTexture(4), 1); // Input Texture (assumed to be at index 0)
	renderContext.Dispatch(commandList, 1920 / 8, 1080 / 8, 1);
	renderContext.TransitionBack(commandList, outputTexture);
	renderContext.TransitionBack(commandList, HTexture(4)); // Input Texture
}

void GrayscalePass::Allocate(DeviceContext* deviceContext)
{
}
