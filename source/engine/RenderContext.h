#pragma once

#include <d3d12.h>
#include <vector>
#include "../externals/d3dx12/d3dx12.h"
#include "DescriptorHeap.h"
#include "CommandList.h"

#pragma comment(lib, "D3DCompiler.lib")

class DeviceContext;
class RenderTarget;
class Texture;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();
	void CreateDescriptorHeap(DeviceContext* deviceContext);
	UINT CreateRenderTarget();
	void CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext);
	UINT CreateRootSignature(DeviceContext* deviceContext);
	UINT CreateShaders(DeviceContext* deviceContext);
	UINT CreatePipelineState(DeviceContext* deviceContext, UINT rootSignatureIndex, UINT shaderIndex);
	UINT CreateViewportAndScissorRect(DeviceContext* deviceContext);
	void CreateVertexBuffer(DeviceContext* deviceContext);
	void CreateIndexBuffer(DeviceContext* deviceContext);
	void CreateConstantBuffer(DeviceContext* deviceContext);
	void CreateSampler(DeviceContext* deviceContext);
	void CreateShader(DeviceContext* deviceContext);
	ID3D12Resource* GetVertexBuffer(UINT index) { return vertexBuffers[index]; }
	ID3D12Resource* GetCurrentBackBuffer();
	void PopulateCommandList(DeviceContext* deviceContext);
	void ExecuteCommandList(UINT cmdListIndex);
	UINT CreateCommandList();
	CommandList* GetCommandList(UINT index) { return commandLists[index]; }
	// Textures
	UINT CreateEmptyTexture(UINT width, UINT height);
	UINT CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name);
	UINT CopyTexture(UINT cmdListIndex, UINT sourceIndex, UINT destIndex);
	// Binding
	void BindRenderTarget(UINT cmdListIndex, UINT rtIndex);
	void CleraRenderTarget(UINT cmdListIndex, UINT rtIndex);
	void ResetCommandList(UINT index);
	void CloseCommandList(UINT index);
	void SetupRenderPass(UINT cmdListIndex, UINT rootSignatureIndex, UINT viewportIndex, UINT scissorsIndex);
	void BindGeometry(UINT cmdListIndex);
	// Barriers
	void TransitionTo(UINT cmdListIndex, UINT textureId, D3D12_RESOURCE_STATES state);
	void TransitionBack(UINT cmdListIndex, UINT textureId);
private:
	DescriptorHeap rtvHeap;
	DescriptorHeap dsvHeap;
	DescriptorHeap cbvSrvUavHeap;
	ID3D12Resource* backBuffer[2];
private:
	std::vector<RenderTarget*> renderTargets;
	std::vector<CommandList*> commandLists;
	std::vector<Texture*> textures;
	std::vector<ID3DBlob*> vertexShaders;
	std::vector<ID3DBlob*> pixelShaders;
	std::vector<ID3D12RootSignature*> rootSignatures;
	std::vector<ID3D12PipelineState*> pipelineStates;
	std::vector<CD3DX12_VIEWPORT> viewports;
	std::vector<CD3DX12_RECT> scissorRects;
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;
	std::vector<ID3D12Resource*> vertexBuffers;
};