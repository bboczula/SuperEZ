#include "Mesh.h"

#include <iostream>

Mesh::Mesh(size_t vbIndexPosition, D3D12_VERTEX_BUFFER_VIEW vbvPosition, size_t vbIndexColor,
	D3D12_VERTEX_BUFFER_VIEW vbvColor, size_t vbIndexTexture, D3D12_VERTEX_BUFFER_VIEW vbvTexture, UINT vertexCount, const char* name)
	: vbIndexPosition(vbIndexPosition), vbvPosition(vbvPosition), vbIndexColor(vbIndexColor), vbvColor(vbvColor), vbIndexTexture(vbIndexTexture), vbvTexture(vbvTexture), vertexCount(vertexCount)
{
	strcpy_s(this->name, name);
}

Mesh::~Mesh()
{
	std::cout << "Mesh Destructor: " << name << std::endl;
}
