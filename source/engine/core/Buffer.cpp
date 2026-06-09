#include "Buffer.h"

Buffer::Buffer(ID3D12Resource* resource, CHAR* name,
	BufferKind kind, UINT sizeInBytes, void* mappedData, size_t cbvDescriptorIndex,
	D3D12_RESOURCE_STATES initialState)
	: resource(resource)
	, currentState(initialState)
	, previousState(initialState)
	, kind(kind)
	, sizeInBytes(sizeInBytes)
	, mappedData(mappedData)
	, cbvDescriptorIndex(cbvDescriptorIndex)
{
	strcpy_s(this->name, name);
}
