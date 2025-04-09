#pragma once

#include <d3d12.h>
#include <vector>
#include "../externals/d3dx12/d3dx12.h"
#include "DescriptorHeap.h"
#include "CommandList.h"

#pragma comment(lib, "D3DCompiler.lib")

class DeviceContext;
class RenderTarget;
class DepthBuffer;
class Texture;
class VertexBuffer;
class Mesh;
class InputLayout;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();
	void CreateDescriptorHeap(DeviceContext* deviceContext);
	void CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext);
	size_t CreateRootSignature(DeviceContext* deviceContext);
	size_t CreateShaders(LPCWSTR shaderName);
	size_t CreatePipelineState(DeviceContext* deviceContext, size_t rootSignatureIndex, size_t shaderIndex, size_t inputLayoutIndex);
	size_t CreateViewportAndScissorRect(DeviceContext* deviceContext);
	size_t CreateInputLayout();
	InputLayout* GetInputLayout(size_t index) { return inputLayouts[index]; }
	void CreateIndexBuffer(DeviceContext* deviceContext);
	void CreateConstantBuffer(DeviceContext* deviceContext);
	void CreateSampler(DeviceContext* deviceContext);
	void CreateShader(DeviceContext* deviceContext);
	ID3D12Resource* GetVertexBuffer(size_t index);
	ID3D12Resource* GetCurrentBackBuffer();
	void ExecuteCommandList(size_t cmdListIndex);
	size_t CreateCommandList();
	CommandList* GetCommandList(size_t index) { return commandLists[index]; }
	// High Level
	size_t CreateRenderTarget();
	size_t CreateDepthBuffer();
	size_t CreateMesh(size_t vbIndexPosition, size_t vbIndexColor, const CHAR* name);
	// Textures
	size_t CreateEmptyTexture(UINT width, UINT height);
	size_t CreateDepthTexture(UINT width, UINT height, const CHAR* name);
	size_t CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name);
	UINT CopyTexture(size_t cmdListIndex, size_t sourceIndex, size_t destIndex);
	// Geometry
	size_t CreateVertexBuffer(UINT numOfVertices, UINT numOfFloatsPerVertex, FLOAT* meshData, const CHAR* name);
	size_t GenerateColors(float* data, size_t size, UINT numOfTriangles, const CHAR* name);
	// Constants
	void SetInlineConstants(size_t cmdListIndex, UINT numOfConstants, void* data);
	// Binding
	void BindRenderTarget(size_t cmdListIndex, size_t rtIndex);
	void BindRenderTargetWithDepth(size_t cmdListIndex, size_t rtIndex, size_t depthIndex);
	void ResetCommandList(size_t index, size_t psoIndex);
	void ResetCommandList(size_t index);
	void CloseCommandList(size_t index);
	void SetupRenderPass(size_t cmdListIndex, size_t psoIndex, size_t rootSignatureIndex, size_t viewportIndex, size_t scissorsIndex);
	void BindGeometry(size_t cmdListIndex, size_t meshIndex);
	// Clearing
	void CleraRenderTarget(size_t cmdListIndex, size_t rtIndex);
	void ClearDepthBuffer(size_t cmdListIndex, size_t depthIndex);
	// Barriers
	void TransitionTo(size_t cmdListIndex, size_t textureId, D3D12_RESOURCE_STATES state);
	void TransitionBack(size_t cmdListIndex, size_t textureId);
	// Drawing
	void DrawMesh(size_t cmdListIndex, size_t meshIndex);
private:
	DescriptorHeap rtvHeap;
	DescriptorHeap dsvHeap;
	DescriptorHeap cbvSrvUavHeap;
	ID3D12Resource* backBuffer[2];
private:
	std::vector<RenderTarget*> renderTargets;
	std::vector<DepthBuffer*> depthBuffers;
	std::vector<CommandList*> commandLists;
	std::vector<Texture*> textures;
	std::vector<VertexBuffer*> vertexBuffers;
	std::vector<Mesh*> meshes;
	std::vector<ID3DBlob*> vertexShaders;
	std::vector<ID3DBlob*> pixelShaders;
	std::vector<ID3D12RootSignature*> rootSignatures;
	std::vector<ID3D12PipelineState*> pipelineStates;
	std::vector<CD3DX12_VIEWPORT> viewports;
	std::vector<CD3DX12_RECT> scissorRects;
	std::vector<InputLayout*> inputLayouts;
};