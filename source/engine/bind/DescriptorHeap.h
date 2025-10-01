#pragma once

#include <d3d12.h>
#include <stack>
#include "../../externals/d3dx12/d3dx12.h"

class DeviceContext;

constexpr uint32_t MAX_NUM_OF_DESCRIPTORS = 90;
constexpr uint32_t MAX_NUM_OF_STATIC_DESCRIPTORS = 10;

class DescriptorHeap
{
public:
	enum HeapPartition
	{
		STATIC,
		DYNAMIC
	};
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, DeviceContext* deviceContext);
	CD3DX12_CPU_DESCRIPTOR_HANDLE Allocate(HeapPartition partition);
	void Allocate(D3D12_CPU_DESCRIPTOR_HANDLE* outCpu, D3D12_GPU_DESCRIPTOR_HANDLE* outGpu);
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu);
	D3D12_CPU_DESCRIPTOR_HANDLE Get(HeapPartition partition, size_t index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPU(HeapPartition partition, size_t index) const;
	uint32_t Size(HeapPartition partition) { return partition == STATIC ? staticSize : staticCapacity + dynamicSize; }
	UINT GetDescriptorSize() { return descriptorSize; }
	ID3D12DescriptorHeap* GetHeap() { return heap; }
	void Reset();
private:
	uint32_t staticCapacity = MAX_NUM_OF_STATIC_DESCRIPTORS;
	uint32_t dynamicCapacity = MAX_NUM_OF_DESCRIPTORS - MAX_NUM_OF_STATIC_DESCRIPTORS;
	D3D12_DESCRIPTOR_HEAP_TYPE heapType;
	ID3D12DescriptorHeap* heap;
	UINT descriptorSize;
	uint32_t staticSize;
	uint32_t dynamicSize;
};