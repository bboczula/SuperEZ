#include "Texture.h"

Texture::Texture(UINT width, UINT height, ID3D12Resource* resource, CHAR* name)
	: width(width), height(height), resource(resource), currentState(D3D12_RESOURCE_STATE_COMMON)
{
	strcpy_s(this->name, name);
}

ID3D12Resource* Texture::GetResource()
{
	return resource;
}
