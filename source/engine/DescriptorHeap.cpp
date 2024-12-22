#include "DescriptorHeap.h"
#include "DeviceContext.h"
#include "Utils.h"

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, DeviceContext* deviceContext)
{
	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.Type = type;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		rtvHeapDesc.NumDescriptors = 1000;
		rtvHeapDesc.Flags |= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		rtvHeapDesc.NumDescriptors = 100;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
		rtvHeapDesc.NumDescriptors = 100;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
		rtvHeapDesc.NumDescriptors = 100;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES:
		break;
	default:
		break;
	}

	ExitIfFailed(deviceContext->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&heap)));
	heap->SetName(L"RTV Heap");
	OutputDebugString(L"Successfully created RTV descriptor heap\n");

	descriptorSize = deviceContext->GetDevice()->GetDescriptorHandleIncrementSize(type);
}
