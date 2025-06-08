#include "DescriptorHeap.h"
#include "DeviceContext.h"
#include "Utils.h"

constexpr uint32_t MAX_NUM_OF_DESCRIPTORS = 100;

void DescriptorHeap::Create(D3D12_DESCRIPTOR_HEAP_TYPE type, DeviceContext* deviceContext)
{
	heapType = type;
	auto isShaderVisibleForHeapType = [] (D3D12_DESCRIPTOR_HEAP_TYPE type) -> bool
	{
		if ((type == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (type == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER))
		{
			return true;
		}
		return false;
	};

	D3D12_DESCRIPTOR_HEAP_DESC heapDesc = {};
	heapDesc.Type = type;
	heapDesc.Flags = isShaderVisibleForHeapType(type) ? D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE : D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	heapDesc.NumDescriptors = MAX_NUM_OF_DESCRIPTORS;

	ExitIfFailed(deviceContext->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
	heap->SetName(L"Descriptor Heap");

	descriptorSize = deviceContext->GetDevice()->GetDescriptorHandleIncrementSize(type);

	size = 0;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Allocate()
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap->GetCPUDescriptorHandleForHeapStart());
	rtvHandle.Offset(size++, descriptorSize);
	assert(size <= MAX_NUM_OF_DESCRIPTORS, "Descriptor heap is full!");

	return rtvHandle;
}

void DescriptorHeap::Allocate(D3D12_CPU_DESCRIPTOR_HANDLE* outCpu, D3D12_GPU_DESCRIPTOR_HANDLE* outGpu)
{
	assert(size < MAX_NUM_OF_DESCRIPTORS && "Descriptor heap is full!");

	*outCpu = Get(size);
	*outGpu = GetGPU(size);
	++size;
}

void DescriptorHeap::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
{
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Get(size_t index) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap->GetCPUDescriptorHandleForHeapStart());
	rtvHandle.Offset(static_cast<INT>(index), descriptorSize);

	return rtvHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPU(size_t index) const
{
	assert((heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER), "GPU handle not supported for non-shader-visible heaps!");

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(heap->GetGPUDescriptorHandleForHeapStart());
	gpuHandle.Offset(static_cast<INT>(index), descriptorSize);
	return gpuHandle;
}

UINT DescriptorHeap::Size()
{
	return size;
}
