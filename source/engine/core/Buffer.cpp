#include "Buffer.h"

Buffer::Buffer(ID3D12Resource* resource, D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout, CHAR* name,
	BufferKind kind, UINT sizeInBytes, void* mappedData, size_t cbvDescriptorIndex,
	D3D12_RESOURCE_STATES initialState)
	: resource(resource)
	, layout(layout)
	, currentState(initialState)
	, previousState(initialState)
	, kind(kind)
	, sizeInBytes(sizeInBytes)
	, mappedData(mappedData)
	, cbvDescriptorIndex(cbvDescriptorIndex)
{
	strcpy_s(this->name, name);
}
