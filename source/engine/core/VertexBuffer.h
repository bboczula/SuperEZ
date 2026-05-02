#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "../../externals/SimpleMath/SimpleMath.h"

class VertexBuffer
{
public:
	VertexBuffer(ID3D12Resource* resource, UINT sizeInBytes, UINT numOfVertices, const char* name,
		DirectX::SimpleMath::Vector3 localMin, DirectX::SimpleMath::Vector3 localMax);
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
	DirectX::SimpleMath::Vector3 GetLocalMin() const { return localMin; }
	DirectX::SimpleMath::Vector3 GetLocalMax() const { return localMax; }
private:
	ID3D12Resource* resource;
	CHAR name[64];
	UINT sizeInBytes;
	UINT numOfVertices;
	DirectX::SimpleMath::Vector3 localMin;
	DirectX::SimpleMath::Vector3 localMax;
};
