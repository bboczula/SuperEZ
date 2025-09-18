#pragma once

#include <Windows.h>
#include <d3d12.h>

enum TextuureLifeSpan
{
	APP,
	SCENE
};

class Texture
{
public:
	Texture(UINT width, UINT height, ID3D12Resource* resource, CHAR* name, size_t srvDescriptorIndex,
		D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON, TextuureLifeSpan span = APP);
	~Texture();
	ID3D12Resource* GetResource();
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
	TextuureLifeSpan GetLifeSpan() const
	{
		return lifeSpan;
	}
private:
	ID3D12Resource* resource;
	D3D12_RESOURCE_STATES currentState;
	D3D12_RESOURCE_STATES previousState;
	TextuureLifeSpan lifeSpan;
	CHAR name[32];
	UINT width;
	UINT height;
	size_t srvDescriptorIndex;
};