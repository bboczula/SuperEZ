#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "../../externals/SimpleMath/SimpleMath.h"

class Mesh
{
public:
	Mesh(size_t vbIndexPosition, D3D12_VERTEX_BUFFER_VIEW vbvPosition, size_t vbIndexColor, D3D12_VERTEX_BUFFER_VIEW vbvColor,
		size_t vbIndexTexture, D3D12_VERTEX_BUFFER_VIEW vbvTexture, size_t vbIndexNormals, D3D12_VERTEX_BUFFER_VIEW vbvNormals,
		UINT vertexCount, DirectX::SimpleMath::Vector3 localMin, DirectX::SimpleMath::Vector3 localMax, const char* name);
	~Mesh();
	D3D12_VERTEX_BUFFER_VIEW GetPositionVertexBufferView() { return vbvPosition; }
	D3D12_VERTEX_BUFFER_VIEW GetColorVertexBufferView() { return vbvColor; }
	D3D12_VERTEX_BUFFER_VIEW GetTextureVertexBufferView() { return vbvTexture; }
	D3D12_VERTEX_BUFFER_VIEW GetNormalsVertexBufferView() { return vbvNormals; }
	UINT GetVertexCount() { return vertexCount; }
	const char* GetName() { return name; }
	DirectX::SimpleMath::Vector3 GetLocalMin() const { return localMin; }
	DirectX::SimpleMath::Vector3 GetLocalMax() const { return localMax; }
private:
	D3D12_VERTEX_BUFFER_VIEW vbvPosition;
	D3D12_VERTEX_BUFFER_VIEW vbvColor;
	D3D12_VERTEX_BUFFER_VIEW vbvTexture;
	D3D12_VERTEX_BUFFER_VIEW vbvNormals;
	CHAR name[32];
	UINT vertexCount;
	DirectX::SimpleMath::Vector3 localMin;
	DirectX::SimpleMath::Vector3 localMax;
};
