#include <d3d12.h>
#include "../externals/d3dx12/d3dx12.h"

class DeviceContext;

class DescriptorHeap
{
public:
	void Create(D3D12_DESCRIPTOR_HEAP_TYPE type, DeviceContext* deviceContext);
	CD3DX12_CPU_DESCRIPTOR_HANDLE Allocate();
	D3D12_CPU_DESCRIPTOR_HANDLE Get(int index);
	UINT Size();
private:
	ID3D12DescriptorHeap* heap;
	UINT descriptorSize;
	UINT size;
};