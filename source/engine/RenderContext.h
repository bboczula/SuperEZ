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
	HRootSignature CreateRootSignature(DeviceContext* deviceContext);
	HShader CreateShaders(LPCWSTR shaderName);
	HPipelineState CreatePipelineState(DeviceContext* deviceContext, HRootSignature rootSignature, HShader shader, HInputLayout inputLayout);
	HViewportAndScissors CreateViewportAndScissorRect(DeviceContext* deviceContext);
	HInputLayout CreateInputLayout();
	InputLayout* GetInputLayout(HInputLayout inputLayout) { return inputLayouts[inputLayout.Index()]; }
	void CreateIndexBuffer(DeviceContext* deviceContext);
	void CreateConstantBuffer(DeviceContext* deviceContext);
	void CreateSampler(DeviceContext* deviceContext);
	void CreateShader(DeviceContext* deviceContext);
	ID3D12Resource* GetCurrentBackBuffer();
	void ExecuteCommandList(HCommandList commandList);
	HCommandList CreateCommandList();
	CommandList* GetCommandList(HCommandList commandList) { return commandLists[commandList.Index()]; }
	// High Level
	HRenderTarget CreateRenderTarget();
	HDepthBuffer CreateDepthBuffer();
	void CreateMesh(HVertexBuffer vbIndexPosition, HVertexBuffer vbIndexColor, const CHAR* name);
	// Textures
	HTexture CreateEmptyTexture(UINT width, UINT height);
	HTexture CreateDepthTexture(UINT width, UINT height, const CHAR* name);
	HTexture CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name);
	void CopyTexture(HCommandList commandList, HTexture source, HTexture destination);
	// Geometry
	HVertexBuffer CreateVertexBuffer(UINT numOfVertices, UINT numOfFloatsPerVertex, FLOAT* meshData, const CHAR* name);
	HVertexBuffer GenerateColors(float* data, size_t size, UINT numOfTriangles, const CHAR* name);
	// Constants
	void SetInlineConstants(HCommandList commandList, UINT numOfConstants, void* data);
	// Binding
	void BindRenderTarget(HCommandList commandList, HRenderTarget renderTarget);
	void BindRenderTargetWithDepth(HCommandList commandList, HRenderTarget renderTarget, HDepthBuffer depthBuffer);
	void ResetCommandList(HCommandList commandList, HPipelineState pipelineState);
	void ResetCommandList(HCommandList commandList);
	void CloseCommandList(HCommandList commandList);
	void SetupRenderPass(HCommandList commandList, HPipelineState pipelineState, HRootSignature rootSignature, HViewportAndScissors viewportAndScissors);
	void BindGeometry(HCommandList commandList, HMesh mesh);
	// Clearing
	void CleraRenderTarget(HCommandList commandList, HRenderTarget renderTarget);
	void ClearDepthBuffer(HCommandList commandList, HDepthBuffer depthBuffer);
	// Barriers
	void TransitionTo(HCommandList commandList, HTexture texture, D3D12_RESOURCE_STATES state);
	void TransitionBack(HCommandList commandList, HTexture texture);
	// Drawing
	void DrawMesh(HCommandList commandList, HMesh mesh);
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