#pragma once

#include <Windows.h>
#include <d3d12.h>

class Buffer
{
public:
	Buffer(ID3D12Resource* resource, D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout, CHAR* name);
	ID3D12Resource* GetResource()
	{
		return resource;
	}
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT GetLayout()
	{
		return layout;
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
private:
	ID3D12Resource* resource;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	D3D12_RESOURCE_STATES currentState;
	D3D12_RESOURCE_STATES previousState;
	CHAR name[32];
};