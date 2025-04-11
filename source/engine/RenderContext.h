#pragma once

#include <d3d12.h>
#include <vector>
#include "../externals/d3dx12/d3dx12.h"
#include "DescriptorHeap.h"
#include "CommandList.h"
#include "Handle.h"

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
	void ExecuteCommandList(HCommandList commandList);
	HCommandList CreateCommandList();
	CommandList* GetCommandList(HCommandList commandList) { return commandLists[commandList.Index()]; }
	// High Level
	HRenderTarget CreateRenderTarget();
	HDepthBuffer CreateDepthBuffer();
	size_t CreateMesh(VertexBufferHandle vbIndexPosition, VertexBufferHandle vbIndexColor, const CHAR* name);
	// Textures
	TextureHandle CreateEmptyTexture(UINT width, UINT height);
	TextureHandle CreateDepthTexture(UINT width, UINT height, const CHAR* name);
	TextureHandle CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name);
	void CopyTexture(HCommandList commandList, TextureHandle source, TextureHandle destination);
	// Geometry
	VertexBufferHandle CreateVertexBuffer(UINT numOfVertices, UINT numOfFloatsPerVertex, FLOAT* meshData, const CHAR* name);
	VertexBufferHandle GenerateColors(float* data, size_t size, UINT numOfTriangles, const CHAR* name);
	// Constants
	void SetInlineConstants(HCommandList commandList, UINT numOfConstants, void* data);
	// Binding
	void BindRenderTarget(HCommandList commandList, HRenderTarget renderTarget);
	void BindRenderTargetWithDepth(HCommandList commandList, HRenderTarget renderTarget, HDepthBuffer depthBuffer);
	void ResetCommandList(HCommandList commandList, size_t psoIndex);
	void ResetCommandList(HCommandList commandList);
	void CloseCommandList(HCommandList commandList);
	void SetupRenderPass(HCommandList commandList, size_t psoIndex, size_t rootSignatureIndex, size_t viewportIndex, size_t scissorsIndex);
	void BindGeometry(HCommandList commandList, size_t meshIndex);
	// Clearing
	void CleraRenderTarget(HCommandList commandList, HRenderTarget renderTarget);
	void ClearDepthBuffer(HCommandList commandList, HDepthBuffer depthBuffer);
	// Barriers
	void TransitionTo(HCommandList commandList, size_t textureId, D3D12_RESOURCE_STATES state);
	void TransitionBack(HCommandList commandList, size_t textureId);
	// Drawing
	void DrawMesh(HCommandList commandList, size_t meshIndex);
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