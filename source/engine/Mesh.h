#pragma once

#include <Windows.h>
#include <d3d12.h>

class Mesh
{
public:
	Mesh(size_t vertexBufferIndex, D3D12_VERTEX_BUFFER_VIEW vertexBufferView, UINT vertexCount, const char* name);
	D3D12_VERTEX_BUFFER_VIEW GetVertexBufferView() { return vertexBufferView; }
	UINT GetVertexCount() { return vertexCount; }
private:
	size_t vertexBufferIndex;
	CHAR name[32];
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	UINT vertexCount;
};