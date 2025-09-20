#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>
#include <vector>

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxguid.lib")

class DeviceContext
{
public:
	DeviceContext();
	~DeviceContext();
	ID3D12Device* GetDevice() { return device; }
	IDXGISwapChain* GetSwapChain();
	ID3D12CommandQueue* GetCommandQueue() { return commandQueue; }
	UINT GetCurrentBackBufferIndex() { return currentBackBufferIndex; }
	void CreateGpuResource(D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
		D3D12_RESOURCE_STATES initResourceState, const IID& riidResource, void** ppResource);
	void CreateUploadResource(D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
		D3D12_RESOURCE_STATES initResourceState, const IID& riidResource, void** ppResource);
	void CreateReadbackResource(D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
		D3D12_RESOURCE_STATES initResourceState, const IID& riidResource, void** ppResource);
	UINT64 GetCopyableFootprintsSize(D3D12_RESOURCE_DESC& resourceDesc, D3D12_PLACED_SUBRESOURCE_FOOTPRINT& layout);
	void Present();
	void Flush();
	void GetMemoryUsage(float& currentMb, float& budgetMb, float& usagePct);
private:
	void CreateDxgiFactory();
	void GetAdapterCapabilities();
	void EnumAndChoseAdapter();
	void CreateDevice();
	void CreateCommandQueue();
	void CreateSwapChain();
	void CreateFence();
	void CreateEventObject();
	void LogAdapterInfo(IDXGIAdapter* adapter);
	void EnableDebugLayer();
	void Signal();
	void WaitForGpu();
	void ReportLiveObjects();
	void SetDebugNames();
	IDXGIFactory2* dxgiFactory2;
	IDXGIAdapter3* dxgiAdapter;
	IDXGISwapChain3* swapChain;
	ID3D12Device* device;
	ID3D12CommandQueue* commandQueue;
	HANDLE fenceEvent;
	UINT64 fenceValue;
	ID3D12Fence* fence;
	UINT currentBackBufferIndex;
};