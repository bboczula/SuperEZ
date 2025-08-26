#pragma once

#include <Windows.h>
#include <d3d12.h>

class Mesh
{
public:
	Mesh(size_t vbIndexPosition, D3D12_VERTEX_BUFFER_VIEW vbvPosition, size_t vbIndexColor, D3D12_VERTEX_BUFFER_VIEW vbvColor,
		size_t vbIndexTexture, D3D12_VERTEX_BUFFER_VIEW vbvTexture, UINT vertexCount, const char* name);
	~Mesh();
	D3D12_VERTEX_BUFFER_VIEW GetPositionVertexBufferView() { return vbvPosition; }
	D3D12_VERTEX_BUFFER_VIEW GetColorVertexBufferView() { return vbvColor; }
	D3D12_VERTEX_BUFFER_VIEW GetTextureVertexBufferView() { return vbvTexture; }
	UINT GetVertexCount() { return vertexCount; }
	const char* GetName() { return name; }
private:
	size_t vbIndexPosition;
	D3D12_VERTEX_BUFFER_VIEW vbvPosition;
	size_t vbIndexColor;
	D3D12_VERTEX_BUFFER_VIEW vbvColor;
	size_t vbIndexTexture;
	D3D12_VERTEX_BUFFER_VIEW vbvTexture;
	CHAR name[32];
	UINT vertexCount;
};