#include "Mesh.h"

#include <iostream>

Mesh::Mesh(HVertexBuffer vbIndexPosition, D3D12_VERTEX_BUFFER_VIEW vbvPosition, HVertexBuffer vbIndexColor,
	D3D12_VERTEX_BUFFER_VIEW vbvColor, HVertexBuffer vbIndexTexture, D3D12_VERTEX_BUFFER_VIEW vbvTexture,
	HVertexBuffer vbIndexNormals, D3D12_VERTEX_BUFFER_VIEW vbvNormals, UINT vertexCount,
	DirectX::SimpleMath::Vector3 localMin, DirectX::SimpleMath::Vector3 localMax, const char* name)
	: vbvPosition(vbvPosition), vbvColor(vbvColor),	vbvTexture(vbvTexture), vbvNormals(vbvNormals),
	vertexCount(vertexCount), localMin(localMin), localMax(localMax), position(vbIndexPosition),
	color(vbIndexColor), texture(vbIndexTexture), normals(vbIndexNormals)
{
	strcpy_s(this->name, name);
}

Mesh::~Mesh()
{
	std::cout << "Mesh Destructor: " << name << std::endl;
}
