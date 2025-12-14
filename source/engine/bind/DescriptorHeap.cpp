#include "DescriptorHeap.h"
#include "../core/DeviceContext.h"
#include "../Utils.h"

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
	heapDesc.NumDescriptors = staticCapacity + dynamicCapacity;

	ExitIfFailed(deviceContext->GetDevice()->CreateDescriptorHeap(&heapDesc, IID_PPV_ARGS(&heap)));
	heap->SetName(L"Descriptor Heap");

	descriptorSize = deviceContext->GetDevice()->GetDescriptorHandleIncrementSize(type);

	dynamicSize = 0;
	staticSize = 0;
}

CD3DX12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Allocate(HeapPartition partition)
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap->GetCPUDescriptorHandleForHeapStart());
	switch (partition)
	{
	case STATIC:
	{
		rtvHandle.Offset(staticSize++, descriptorSize);
		assert(staticSize <= staticCapacity, "Static Descriptor Heap partition is full!");
		break;
	}
	case DYNAMIC:
	{
		rtvHandle.Offset(staticCapacity + dynamicSize++, descriptorSize);
		assert(dynamicSize <= dynamicCapacity, "Dynamic Descriptor Heap partition is full!");
		break;
	}
	}

	return rtvHandle;
}

void DescriptorHeap::Allocate(D3D12_CPU_DESCRIPTOR_HANDLE* outCpu, D3D12_GPU_DESCRIPTOR_HANDLE* outGpu)
{
	Allocate(HeapPartition::STATIC);
	*outCpu = Get(HeapPartition::STATIC, staticSize - 1);
	*outGpu = GetGPU(HeapPartition::STATIC, staticSize - 1);
	// Is this a correct?
	//++dynamicSize;
}

void DescriptorHeap::Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu)
{
}

D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::Get(HeapPartition partition, size_t index) const
{
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(heap->GetCPUDescriptorHandleForHeapStart());
	switch (partition)
	{
	case STATIC:
	{
		rtvHandle.Offset(static_cast<INT>(index), descriptorSize);
		break;
	}
	case DYNAMIC:
	{
		// Here, we shouldn't care about the offset, index is not relative
		rtvHandle.Offset(static_cast<INT>(index), descriptorSize);
		break;
	}
	};

	return rtvHandle;
}

D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPU(HeapPartition partition, size_t index) const
{
	assert((heapType == D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV) || (heapType == D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER),
		"GPU handle not supported for non-shader-visible heaps!");

	CD3DX12_GPU_DESCRIPTOR_HANDLE gpuHandle(heap->GetGPUDescriptorHandleForHeapStart());
	switch (partition)
	{
	case STATIC:
	{
		gpuHandle.Offset(static_cast<INT>(index), descriptorSize);
		break;
	}
	case DYNAMIC:
	{
		gpuHandle.Offset(static_cast<INT>(index), descriptorSize);
		break;
	}
	}
	return gpuHandle;
}

void DescriptorHeap::Reset()
{
	dynamicSize = 0;
}
