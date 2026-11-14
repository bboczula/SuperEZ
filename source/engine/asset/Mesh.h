#pragma once

#include <Windows.h>
#include <d3d12.h>
#include "../../externals/SimpleMath/SimpleMath.h"
#include "Handle.h"

class Mesh
{
public:
	Mesh(HVertexBuffer vbIndexPosition, D3D12_VERTEX_BUFFER_VIEW vbvPosition, HVertexBuffer vbIndexColor, D3D12_VERTEX_BUFFER_VIEW vbvColor,
		HVertexBuffer vbIndexTexture, D3D12_VERTEX_BUFFER_VIEW vbvTexture, HVertexBuffer vbIndexNormals, D3D12_VERTEX_BUFFER_VIEW vbvNormals,
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
	bool HasPosition() const { return position.IsValid(); }
	bool HasColor() const { return color.IsValid(); }
	bool HasTexture() const { return texture.IsValid(); }
	bool HasNormals() const { return normals.IsValid(); }
private:
	HVertexBuffer position;
	D3D12_VERTEX_BUFFER_VIEW vbvPosition;
	HVertexBuffer color;
	D3D12_VERTEX_BUFFER_VIEW vbvColor;
	HVertexBuffer texture;
	D3D12_VERTEX_BUFFER_VIEW vbvTexture;
	HVertexBuffer normals;
	D3D12_VERTEX_BUFFER_VIEW vbvNormals;
	CHAR name[32];
	UINT vertexCount;
	DirectX::SimpleMath::Vector3 localMin;
	DirectX::SimpleMath::Vector3 localMax;
};
