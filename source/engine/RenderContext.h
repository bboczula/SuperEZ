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
	UINT CreateRenderTarget(DeviceContext* deviceContext);
	void CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext);
	UINT CreateRootSignature(DeviceContext* deviceContext);
	UINT CreateShaders(DeviceContext* deviceContext);
	UINT CreatePipelineState(DeviceContext* deviceContext, UINT rootSignatureIndex, UINT shaderIndex);
	UINT CreateViewportAndScissorRect(DeviceContext* deviceContext);
	void CreateVertexBuffer(DeviceContext* deviceContext);
	void CreateIndexBuffer(DeviceContext* deviceContext);
	void CreateConstantBuffer(DeviceContext* deviceContext);
	UINT CreateTexture(DeviceContext* deviceContext);
	void CreateSampler(DeviceContext* deviceContext);
	void CreateShader(DeviceContext* deviceContext);
	ID3D12Resource* GetVertexBuffer(UINT index) { return vertexBuffers[index]; }
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
private:
	std::vector<RenderTarget*> renderTargets;
	std::vector<ID3DBlob*> vertexShaders;
	std::vector<ID3DBlob*> pixelShaders;
	std::vector<ID3D12RootSignature*> rootSignatures;
	std::vector<ID3D12PipelineState*> pipelineStates;
	std::vector<CD3DX12_VIEWPORT> viewports;
	std::vector<CD3DX12_RECT> scissorRects;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	std::vector<ID3D12Resource*> vertexBuffers;
};