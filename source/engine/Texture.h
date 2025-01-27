#pragma once

#include <Windows.h>
#include <d3d12.h>

class Texture
{
public:
	Texture(UINT width, UINT height, ID3D12Resource* resource, CHAR* name, D3D12_RESOURCE_STATES initState = D3D12_RESOURCE_STATE_COMMON);
	ID3D12Resource* GetResource();
	D3D12_RESOURCE_STATES GetCurrentState()
	{
		return currentState;
	}
	void SetCurrentState(D3D12_RESOURCE_STATES state)
	{
		currentState = state;
	}
private:
	ID3D12Resource* resource;
	D3D12_RESOURCE_STATES currentState;
	CHAR name[32];
	UINT width;
	UINT height;
};