#include "BlitPass.h"
#include "DeviceContext.h"
#include "RenderContext.h"
#include "../externals/d3dx12/d3dx12.h"


extern DeviceContext deviceContext;
extern RenderContext renderContext;

BlitPass::BlitPass()
{
}

BlitPass::~BlitPass()
{
}

void BlitPass::Prepare()
{
}

void BlitPass::Execute()
{
	renderContext.ResetCommandList(commandListIndex);
	renderContext.TransitionTo(commandListIndex, 2, D3D12_RESOURCE_STATE_COPY_SOURCE);
	
	auto frameIndex = deviceContext.GetCurrentBackBufferIndex();
	renderContext.TransitionTo(commandListIndex, frameIndex, D3D12_RESOURCE_STATE_COPY_DEST);
	renderContext.CopyTexture(commandListIndex, 2, frameIndex);
	renderContext.TransitionTo(commandListIndex, frameIndex, D3D12_RESOURCE_STATE_PRESENT);

	renderContext.CloseCommandList(commandListIndex);
	renderContext.ExecuteCommandList(commandListIndex);
}

void BlitPass::Allocate(DeviceContext* deviceContext)
{
}