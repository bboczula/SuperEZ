#include "VertexBuffer.h"

#include <string>

VertexBuffer::VertexBuffer(ID3D12Resource* resource, UINT sizeInBytes, UINT numOfVertices, const char* name)
	: resource(resource), sizeInBytes(sizeInBytes), numOfVertices(numOfVertices)
{
	strcpy_s(this->name, name);
}

VertexBuffer::~VertexBuffer()
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
	OutputDebugStringA(("VertexBuffer Destructor: " + std::string(name) + "\n").c_str());
	// Cleanup if necessary
	// For example, release resources or perform other cleanup tasks
}