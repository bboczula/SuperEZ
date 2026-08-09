#pragma once

#include <Windows.h>
#include <d3d12.h>

#include <vector>

enum TextuureLifeSpan
{
	APP,
	SCENE
};

struct TextureMipDesc
{
	UINT width = 1;
	UINT height = 1;
};

std::vector<TextureMipDesc> CalculateTextureMipChain(UINT width, UINT height);

class Texture
{
public:
	Texture(UINT width, UINT height, UINT mipLevels, ID3D12Resource* resource, CHAR* name, size_t srvDescriptorIndex,
		D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON, TextuureLifeSpan span = APP);
	~Texture();
	ID3D12Resource* GetResource();
	UINT GetMipLevels() const
	{
		return mipLevels;
	}
	D3D12_RESOURCE_STATES GetCurrentState()
	{
		return currentState;
	}
	D3D12_RESOURCE_STATES GetPreviousState()
	{
		return previousState;
	}
	void SetCurrentState(D3D12_RESOURCE_STATES state)
	{
		previousState = currentState;
		currentState = state;
	}
	size_t GetSrvDescriptorIndex() const
	{
		return srvDescriptorIndex;
	}
	void SetSrgbSrvDescriptorIndex(size_t index)
	{
		srgbSrvDescriptorIndex = index;
		hasSrgbSrv = true;
	}
	size_t GetColorSrvDescriptorIndex(bool useSrgb) const
	{
		return useSrgb && hasSrgbSrv ? srgbSrvDescriptorIndex : srvDescriptorIndex;
	}
	void SetUavDescriptorIndex(size_t index)
	{
		uavDescriptorIndex = index;
	}
	size_t GetUavDescriptorIndex() const
	{
		return uavDescriptorIndex;
	}
	TextuureLifeSpan GetLifeSpan() const
	{
		return lifeSpan;
	}
	const CHAR* GetName() const
	{
		return name;
	}
private:
	ID3D12Resource* resource;
	D3D12_RESOURCE_STATES currentState;
	D3D12_RESOURCE_STATES previousState;
	TextuureLifeSpan lifeSpan;
	CHAR name[32];
	UINT width;
	UINT height;
	UINT mipLevels;
	size_t srvDescriptorIndex;
	size_t srgbSrvDescriptorIndex = 0;
	bool hasSrgbSrv = false;
	size_t uavDescriptorIndex;
};
