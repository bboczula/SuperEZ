#pragma once

#include <Windows.h>
#include <d3d12.h>

class VertexBuffer
{
public:
	VertexBuffer(ID3D12Resource* resource, UINT sizeInBytes, UINT numOfVertices, const char* name);
	~VertexBuffer();
	ID3D12Resource* GetResource()
	{
		return resource;
	}
	UINT GetSizeInBytes()
	{
		return sizeInBytes;
	}
	UINT GetNumOfVertices()
	{
		return numOfVertices;
	}
private:
	ID3D12Resource* resource;
	CHAR name[64];
	UINT sizeInBytes;
	UINT numOfVertices;
};