#include "BlitPass.h"
#include "DeviceContext.h"
#include "RenderContext.h"
#include "../externals/d3dx12/d3dx12.h"
#include "pix3.h"

extern DeviceContext deviceContext;
extern RenderContext renderContext;

BlitPass::BlitPass() : RenderPass(L"Blit", Type::Drawless)
{
}

BlitPass::~BlitPass()
{
}

void BlitPass::ConfigurePipelineState()
{
}

void BlitPass::Prepare()
{
}

void BlitPass::Execute()
{
	renderContext.TransitionTo(commandListIndex, 2, D3D12_RESOURCE_STATE_COPY_SOURCE);
	
	auto frameIndex = deviceContext.GetCurrentBackBufferIndex();
	TextureHandle renderTarget = TextureHandle(2);
	TextureHandle backBuffer = TextureHandle(frameIndex);
	renderContext.TransitionTo(commandListIndex, frameIndex, D3D12_RESOURCE_STATE_COPY_DEST);
	renderContext.CopyTexture(commandListIndex, renderTarget, backBuffer);

	renderContext.TransitionBack(commandListIndex, 2);
	renderContext.TransitionTo(commandListIndex, frameIndex, D3D12_RESOURCE_STATE_PRESENT);
}

void BlitPass::Allocate(DeviceContext* deviceContext)
{
}