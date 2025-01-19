#include "CommandList.h"
#include "Utils.h"
#include "DeviceContext.h"

extern DeviceContext deviceContext;

CommandList::CommandList()
{
}

CommandList::~CommandList()
{
	SafeRelease(&commandList);
	SafeRelease(&commandAllocator);
}

void CommandList::Create()
{
	OutputDebugString(L"CreateCommandList\n");

	ExitIfFailed(deviceContext.GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
	commandAllocator->SetName(L"Render Context Command Allocator");
	ExitIfFailed(deviceContext.GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
	commandList->SetName(L"Render Context Command List");
	commandList->Close();

	OutputDebugString(L"CreateCommandList succeeded\n");
}

void CommandList::Reset(ID3D12PipelineState* pso)
{
	ExitIfFailed(commandAllocator->Reset());
	ExitIfFailed(commandList->Reset(commandAllocator, pso));
}

void CommandList::Close()
{
	ExitIfFailed(commandList->Close());
}
