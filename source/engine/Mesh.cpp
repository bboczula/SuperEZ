#include "Mesh.h"

Mesh::Mesh(size_t vertexBufferIndex, D3D12_VERTEX_BUFFER_VIEW vertexBufferView, const char* name)
	: vertexBufferIndex(vertexBufferIndex), vertexBufferView(vertexBufferView)
{
	strcpy_s(this->name, name);
}
