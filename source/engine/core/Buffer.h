#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <cstdint>

enum class BufferKind : uint8_t
{
	Generic,
	Readback,
	TextureUpload,
	Constant
};

class Buffer
{
public:
	static constexpr size_t InvalidDescriptorIndex = static_cast<size_t>(-1);
	Buffer(ID3D12Resource* resource, D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout, CHAR* name, BufferKind kind = BufferKind::Generic, UINT sizeInBytes = 0,
		void* mappedData = nullptr, size_t cbvDescriptorIndex = InvalidDescriptorIndex,
		D3D12_RESOURCE_STATES initialState = D3D12_RESOURCE_STATE_COMMON);
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
	bool HasCBV() const { return cbvDescriptorIndex != InvalidDescriptorIndex; }
	size_t GetCBVDescriptorIndex() const { return cbvDescriptorIndex; }
	void* GetMappedData() const { return mappedData; }
	UINT GetSizeInBytes() const { return sizeInBytes; }
	BufferKind GetKind() const { return kind; }
private:
	ID3D12Resource* resource;
	D3D12_PLACED_SUBRESOURCE_FOOTPRINT layout;
	D3D12_RESOURCE_STATES currentState;
	D3D12_RESOURCE_STATES previousState;
	CHAR name[32];
	BufferKind kind;
	UINT sizeInBytes;
	void* mappedData;
	size_t cbvDescriptorIndex;
};
