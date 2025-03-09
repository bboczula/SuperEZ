#include "DescriptorHeap.h"
#include "DeviceContext.h"
#include "Utils.h"

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, DeviceContext* deviceContext)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = type;
	rtvHeapDesc.Flags = (type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	rtvHeapDesc.NumDescriptors = 100;

	ExitIfFailed(deviceContext->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&heap)));
	heap->SetName(L"Descriptor Heap");

	descriptorSize = deviceContext->GetDevice()->GetDescriptorHandleIncrementSize(type);

	size = 0;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Allocate()
{
	// Here you can add a check to see if the heap is full
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap->GetCPUDescriptorHandleForHeapStart());
	rtvHandle.Offset(size++, descriptorSize);

	return rtvHandle;
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Get(size_t index)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap->GetCPUDescriptorHandleForHeapStart());
	rtvHandle.Offset(index, descriptorSize);

	return rtvHandle;
}

UINT DescriptorHeap::Size()
{
	return size;
}
