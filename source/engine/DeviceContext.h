#pragma once

#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_4.h>

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
	void Present();
	void Flush();
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
	// Fields
	IDXGIFactory2* dxgiFactory2;
	IDXGIAdapter* dxgiAdapter;
	IDXGISwapChain3* swapChain;
	ID3D12Device* device;
	ID3D12CommandQueue* commandQueue;
	HANDLE fenceEvent;
	UINT64 fenceValue;
	ID3D12Fence* fence;
	UINT currentBackBufferIndex;
};