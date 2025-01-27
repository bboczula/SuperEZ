#include "Texture.h"

Texture::Texture(UINT width, UINT height, ID3D12Resource* resource, CHAR* name, D3D12_RESOURCE_STATES initState)
	: width(width), height(height), resource(resource), currentState(initState)
{
	strcpy_s(this->name, name);
}

ID3D12Resource* Texture::GetResource()
{
	return resource;
}
