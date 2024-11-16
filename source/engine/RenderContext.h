#pragma once

#include <d3d12.h>
#include <vector>
#include "../externals/d3dx12/d3dx12.h"

#pragma comment(lib, "D3DCompiler.lib")

class DeviceContext;
class RenderTarget;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();
	void CreateDescriptorHeap(DeviceContext* deviceContext);
	void CreateCommandBuffer(DeviceContext* deviceContext);
	void CreateRenderTarget(DeviceContext* deviceContext);
	void CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext);
	void CreateRootSignature(DeviceContext* deviceContext);
	void CreateShaders(DeviceContext* deviceContext);
	void CreatePipelineState(DeviceContext* deviceContext);
	void CreateViewportAndScissorRect(DeviceContext* deviceContext);
	void CreateVertexBuffer(DeviceContext* deviceContext);
	void CreateIndexBuffer(DeviceContext* deviceContext);
	void CreateConstantBuffer(DeviceContext* deviceContext);
	void CreateTexture(DeviceContext* deviceContext);
	void CreateSampler(DeviceContext* deviceContext);
	void CreateShader(DeviceContext* deviceContext);
	void PopulateCommandList(DeviceContext* deviceContext);
	void ExecuteCommandList(DeviceContext* deviceContext);
private:
	ID3D12DescriptorHeap* rtvHeap;
	ID3D12DescriptorHeap* dsvHeap;
	ID3D12DescriptorHeap* cbvSrvUavHeap;
	UINT rtvHeapDescriptorSize;
	UINT dsvHeapDescriptorSize;
	UINT cbvSrvUavHeapDescriptorSize;
	ID3D12CommandAllocator* commandAllocator;
	ID3D12GraphicsCommandList* commandList;
	// Need to get rid of this magic number
	ID3D12Resource* backBuffer[2];
	ID3D12RootSignature* rootSignature;
	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;
	ID3D12PipelineState* pipelineState;
	CD3DX12_VIEWPORT viewport;
	CD3DX12_RECT scissorRect;
private:
	std::vector<RenderTarget*> renderTargets;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
};