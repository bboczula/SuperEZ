#include "Texture.h"

#include <string>

Texture::Texture(UINT width, UINT height, ID3D12Resource* resource, CHAR* name, size_t srvDescriptorIndex, D3D12_RESOURCE_STATES initState, TextuureLifeSpan span)
	: width(width), height(height), resource(resource), currentState(initState), srvDescriptorIndex(srvDescriptorIndex), lifeSpan(span)
{
	strcpy_s(this->name, name);
}

Texture::~Texture()
{
	if (resource)
	{
		resource->Release();
		resource = nullptr;
	}
	
	OutputDebugStringA(("Texture Destructor: " + std::string(name) + "\n").c_str());
}

ID3D12Resource* Texture::GetResource()
{
	return resource;
}
