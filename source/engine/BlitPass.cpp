#include "BlitPass.h"
#include "DeviceContext.h"
#include "RenderContext.h"
#include "../externals/d3dx12/d3dx12.h"
#include "pix3.h"

extern DeviceContext deviceContext;
extern RenderContext renderContext;

BlitPass::BlitPass() : RenderPass(L"Blit")
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
	//renderContext.ResetCommandList(commandListIndex);

	//PIXBeginEvent(renderContext.GetCommandList(commandListIndex)->GetCommandList(), 0, L"Blit Pass");
	renderContext.TransitionTo(commandListIndex, 2, D3D12_RESOURCE_STATE_COPY_SOURCE);
	
	auto frameIndex = deviceContext.GetCurrentBackBufferIndex();
	renderContext.TransitionTo(commandListIndex, frameIndex, D3D12_RESOURCE_STATE_COPY_DEST);
	renderContext.CopyTexture(commandListIndex, 2, frameIndex);

	renderContext.TransitionBack(commandListIndex, 2);
	renderContext.TransitionTo(commandListIndex, frameIndex, D3D12_RESOURCE_STATE_PRESENT);
	//PIXEndEvent(renderContext.GetCommandList(commandListIndex)->GetCommandList());
	//
	//renderContext.CloseCommandList(commandListIndex);
	//renderContext.ExecuteCommandList(commandListIndex);
}

void BlitPass::Allocate(DeviceContext* deviceContext)
{
}