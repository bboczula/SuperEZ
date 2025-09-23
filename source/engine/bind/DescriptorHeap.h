#pragma once

#include <d3d12.h>
#include <stack>
#include "../../externals/d3dx12/d3dx12.h"

class DeviceContext;

class DescriptorHeap
{
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, DeviceContext* deviceContext);
	CD3DX12_CPU_DESCRIPTOR_HANDLE Allocate();
	void Allocate(D3D12_CPU_DESCRIPTOR_HANDLE* outCpu, D3D12_GPU_DESCRIPTOR_HANDLE* outGpu);
	void Free(D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu);
	D3D12_CPU_DESCRIPTOR_HANDLE Get(size_t index) const;
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPU(size_t index) const;
	UINT Size();
	UINT GetDescriptorSize() { return descriptorSize; }
	ID3D12DescriptorHeap* GetHeap() { return heap; }
	void Reset();
private:
	D3D12_DESCRIPTOR_HEAP_TYPE heapType;
	ID3D12DescriptorHeap* heap;
	UINT descriptorSize;
	UINT size;
};