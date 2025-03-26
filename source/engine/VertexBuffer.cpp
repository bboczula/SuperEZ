#include "VertexBuffer.h"

VertexBuffer::VertexBuffer(ID3D12Resource* resource, UINT sizeInBytes, UINT numOfVertices, const char* name)
	: resource(resource), sizeInBytes(sizeInBytes), numOfVertices(numOfVertices)
{
	strcpy_s(this->name, name);
}

VertexBuffer::~VertexBuffer()
{
}