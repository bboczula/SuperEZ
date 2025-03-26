#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(ID3D12Resource* resource, UINT sizeInBytes, const char* name)
	: resource(resource), sizeInBytes(sizeInBytes)
{
	strcpy_s(this->name, name);
}

VertexBuffer::~VertexBuffer()
{
}