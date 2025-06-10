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

void BlitPass::Initialize()
{
}

void BlitPass::Execute()
{
	renderContext.TransitionTo(commandList, HTexture(2), D3D12_RESOURCE_STATE_COPY_SOURCE);
	
	auto frameIndex = deviceContext.GetCurrentBackBufferIndex();
	HTexture renderTarget = HTexture(2);
	HTexture backBuffer = HTexture(frameIndex);
	HTexture frameTexture = HTexture(frameIndex);
	renderContext.TransitionTo(commandList, frameTexture, D3D12_RESOURCE_STATE_COPY_DEST);
	renderContext.CopyTexture(commandList, renderTarget, backBuffer);

	renderContext.TransitionBack(commandList, renderTarget);
	renderContext.TransitionTo(commandList, frameTexture, D3D12_RESOURCE_STATE_PRESENT);
}

void BlitPass::Allocate(DeviceContext* deviceContext)
{
}