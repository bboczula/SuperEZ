#pragma once

#include <Windows.h>
#include <d3d12.h>

class Mesh
{
public:
	Mesh(size_t vbIndexPosition, D3D12_VERTEX_BUFFER_VIEW vbvPosition, size_t vbIndexColor, D3D12_VERTEX_BUFFER_VIEW vbvColor, UINT vertexCount, const char* name);
	D3D12_VERTEX_BUFFER_VIEW GetPositionVertexBufferView() { return vbvPosition; }
	D3D12_VERTEX_BUFFER_VIEW GetColorVertexBufferView() { return vbvColor; }
	UINT GetVertexCount() { return vertexCount; }
private:
	size_t vbIndexPosition;
	D3D12_VERTEX_BUFFER_VIEW vbvPosition;
	size_t vbIndexColor;
	D3D12_VERTEX_BUFFER_VIEW vbvColor;
	CHAR name[32];
	UINT vertexCount;
};