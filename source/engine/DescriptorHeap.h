#include <d3d12.h>
#include "../externals/d3dx12/d3dx12.h"

class DescriptorHeap
{
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, DeviceContext* deviceContext);
	CD3DX12_CPU_DESCRIPTOR_HANDLE Allocate();
private:
	ID3D12DescriptorHeap* heap;
	UINT descriptorSize;
};