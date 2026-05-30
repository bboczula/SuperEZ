#include "Texture.h"

#include <string>

namespace
{
	UINT MaxUint(UINT a, UINT b)
	{
		return a > b ? a : b;
	}
}

UINT CalculateTextureMipCount(UINT width, UINT height)
{
	UINT maxDimension = MaxUint(MaxUint(width, height), 1u);
	UINT mipCount = 1;

	while (maxDimension > 1)
	{
		maxDimension /= 2;
		++mipCount;
	}

	return mipCount;
}

std::vector<TextureMipDesc> CalculateTextureMipChain(UINT width, UINT height)
{
	std::vector<TextureMipDesc> mipChain;

	width = MaxUint(width, 1u);
	height = MaxUint(height, 1u);

	while (true)
	{
		mipChain.push_back({ width, height });

		if (width == 1 && height == 1)
		{
			break;
		}

		width = MaxUint(width / 2, 1u);
		height = MaxUint(height / 2, 1u);
	}

	return mipChain;
}

Texture::Texture(UINT width, UINT height, UINT mipLevels, ID3D12Resource* resource, CHAR* name, size_t srvDescriptorIndex, D3D12_RESOURCE_STATES initState, TextuureLifeSpan span)
	: width(width), height(height), mipLevels(mipLevels), resource(resource), currentState(initState), srvDescriptorIndex(srvDescriptorIndex), lifeSpan(span)
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
