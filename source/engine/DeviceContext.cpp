#include "DeviceContext.h"
#include "WindowContext.h"
#include "debugapi.h"
#include "Utils.h"
#include "../externals/SimpleMath/SimpleMath.h"
#include "../externals/d3dx12/d3dx12.h"

#include <stdio.h>
#include <dxgidebug.h>
#include <d3d12.h>

extern WindowContext windowContext;

DeviceContext::DeviceContext() : fenceValue(1), currentBackBufferIndex(0)
{
	OutputDebugString(L"DeviceContext Constructor\n");
	CreateDxgiFactory();
	GetAdapterCapabilities();
	EnumAndChoseAdapter();
	CreateDevice();
	CreateCommandQueue();
	CreateSwapChain();
	CreateFence();
	CreateEventObject();
	SetDebugNames();
	Flush();
}

DeviceContext::~DeviceContext()
{
	OutputDebugString(L"DeviceContext Destructor\n");

	SafeRelease(&fence);
	SafeRelease(&swapChain);
	SafeRelease(&commandQueue);
	SafeRelease(&device);
	SafeRelease(&dxgiAdapter);

	// This is a last destructor for DirectX related things
	ReportLiveObjects();
}

IDXGISwapChain* DeviceContext::GetSwapChain()
{
	return swapChain;
}

void DeviceContext::Present()
{
	swapChain->Present(1, 0);
	currentBackBufferIndex = swapChain->GetCurrentBackBufferIndex();
	//Sleep(500);
}

void DeviceContext::CreateDxgiFactory()
{
	ExitIfFailed(CreateDXGIFactory2(0, IID_PPV_ARGS(&dxgiFactory2)));
	OutputDebugString(L"Successfully created DXGI Factory\n");
}

void DeviceContext::GetAdapterCapabilities()
{
	OutputDebugString(L"GetAdapterCapabilities (not used)\n");
}

void DeviceContext::EnumAndChoseAdapter()
{
	OutputDebugString(L"EnumAndChoseAdapter\n");
	IDXGIAdapter* currentAdapter;
	UINT index = 0;
	while (1)
	{
		HRESULT result = dxgiFactory2->EnumAdapters(index++, &currentAdapter);
		if (result == DXGI_ERROR_NOT_FOUND)
		{
			break;
		}
		ExitIfFailed(result);
		LogAdapterInfo(currentAdapter);

		SafeRelease(&currentAdapter);
	}

	// Finally we pick the first adapter
	ExitIfFailed(dxgiFactory2->EnumAdapters(0, &dxgiAdapter));
	OutputDebugString(L"Successfully picked adapter\n");
}

void DeviceContext::CreateDevice()
{
	OutputDebugString(L"CreateDevice\n");
	EnableDebugLayer();

	ExitIfFailed(D3D12CreateDevice(dxgiAdapter, D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
	OutputDebugString(L"Successfully created the device\n");
}

void DeviceContext::CreateCommandQueue()
{
	OutputDebugString(L"CreateCommandQueue\n");
	D3D12_COMMAND_QUEUE_DESC commandQueueDesc;
	ZeroMemory(&commandQueueDesc, sizeof(commandQueueDesc));

	commandQueueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	commandQueueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	commandQueueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	commandQueueDesc.NodeMask = 0;

	ExitIfFailed(device->CreateCommandQueue(&commandQueueDesc, IID_PPV_ARGS(&commandQueue)));
	OutputDebugString(L"Successfully created command queue\n");
}

void DeviceContext::CreateSwapChain()
{
	OutputDebugString(L"CreateSwapChain\n");
	auto windowHandle = windowContext.GetWindowHandle();

	DXGI_SAMPLE_DESC sampleDesc;
	ZeroMemory(&sampleDesc, sizeof(sampleDesc));
	sampleDesc.Count = 1;
	sampleDesc.Quality = 0;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));
	swapChainDesc.Width = windowContext.GetWidth();
	swapChainDesc.Height = windowContext.GetHeight();
	swapChainDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = FRAME_COUNT;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.SampleDesc = sampleDesc;

	IDXGISwapChain1* tempSwapChain = nullptr;
	ExitIfFailed(dxgiFactory2->CreateSwapChainForHwnd(commandQueue, windowHandle, &swapChainDesc, nullptr, nullptr, &tempSwapChain));
	OutputDebugString(L"Successfully created swap chain\n");

	tempSwapChain->QueryInterface(IID_PPV_ARGS(&swapChain));
	tempSwapChain->Release();
}

void DeviceContext::CreateFence()
{
	OutputDebugString(L"CreateFence\n");
	ExitIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
}

void DeviceContext::CreateEventObject()
{
	OutputDebugString(L"CreateEventObject\n");
	fenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (fenceEvent == nullptr)
	{
		ExitIfFailed(HRESULT_FROM_WIN32(GetLastError()));
	}
}

void DeviceContext::Flush()
{
	Signal();
	WaitForGpu();
}

void DeviceContext::LogAdapterInfo(IDXGIAdapter* adapter)
{
	DXGI_ADAPTER_DESC adapterDesc;
	adapter->GetDesc(&adapterDesc);
	char buf[256];
	snprintf(buf, sizeof(buf), "%ws%s%zu%s", adapterDesc.Description, " (Dedicated VRAM: ", adapterDesc.DedicatedVideoMemory / 1024 / 1024, " MB)\n");
	OutputDebugStringA(buf);
}

void DeviceContext::EnableDebugLayer()
{
	ID3D12Debug* debugInterface_0;
	ExitIfFailed(D3D12GetDebugInterface(IID_PPV_ARGS(&debugInterface_0)));
	debugInterface_0->EnableDebugLayer();

	ID3D12Debug1* debugInterface_1;
	ExitIfFailed(debugInterface_0->QueryInterface(IID_PPV_ARGS(&debugInterface_1)));
	debugInterface_1->SetEnableGPUBasedValidation(true);
}

void DeviceContext::Signal()
{
	ExitIfFailed(commandQueue->Signal(fence, fenceValue));
}

void DeviceContext::WaitForGpu()
{
	// Wait until the previous frame is finished.
	if (fence->GetCompletedValue() < fenceValue)
	{
		ExitIfFailed(fence->SetEventOnCompletion(fenceValue, fenceEvent));
		WaitForSingleObject(fenceEvent, INFINITE);
	}
	fenceValue++;
}

void DeviceContext::ReportLiveObjects()
{
	// The deebug layer needs to be enabled for this to work
	IDXGIDebug* dxgiDebugInterface;
	if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(&dxgiDebugInterface))))
	{
		dxgiDebugInterface->ReportLiveObjects(DXGI_DEBUG_ALL,
			DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	}
	SafeRelease(&dxgiDebugInterface);
}

void DeviceContext::SetDebugNames()
{
	ExitIfFailed(device->SetName(L"Device Context Device"));
	ExitIfFailed(commandQueue->SetName(L"Device Context Direct Command Queue"));
	ExitIfFailed(fence->SetName(L"Device Context Fence"));
}

void DeviceContext::CreateResource(D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
	D3D12_RESOURCE_STATES initResourceState, const IID &riidResource, void** ppResource)
{
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(&heapProperties, heapFlags, desc, initResourceState, nullptr, riidResource, ppResource);
}

void DeviceContext::CreateUploadResource(D3D12_HEAP_FLAGS heapFlags, const D3D12_RESOURCE_DESC* desc,
	D3D12_RESOURCE_STATES initResourceState, const IID& riidResource, void** ppResource)
{
	CD3DX12_HEAP_PROPERTIES heapProperties(D3D12_HEAP_TYPE_UPLOAD);
	device->CreateCommittedResource(&heapProperties, heapFlags, desc, initResourceState, nullptr, riidResource, ppResource);
}