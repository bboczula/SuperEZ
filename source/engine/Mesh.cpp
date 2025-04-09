#include "Mesh.h"

Mesh::Mesh(size_t vbIndexPosition, D3D12_VERTEX_BUFFER_VIEW vbvPosition, size_t vbIndexColor,
	D3D12_VERTEX_BUFFER_VIEW vbvColor, UINT vertexCount, const char* name)
	: vbIndexPosition(vbIndexPosition), vbvPosition(vbvPosition), vbIndexColor(vbIndexColor), vbvColor(vbvColor), vertexCount(vertexCount)
{
	strcpy_s(this->name, name);
}
