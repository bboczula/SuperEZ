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
class Buffer;
class VertexBuffer;
class Mesh;
class Material;
class InputLayout;
class Shader;

class RenderContext
{
public:
	RenderContext();
	~RenderContext();
	void CreateDescriptorHeap(DeviceContext* deviceContext);
	void CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext);
	HRootSignature CreateRootSignature(DeviceContext* deviceContext);
	HShader CreateShader(LPCWSTR shaderFileName, LPCSTR entryPoint, LPCSTR shaderModel);
	HPipelineState CreatePipelineState(DeviceContext* deviceContext, HRootSignature rootSignature, HShader vertexShader, HShader pixelShader, HInputLayout inputLayout);
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
	UINT GetNumOfMeshes() { return static_cast<UINT>(meshes.size()); }
	DescriptorHeap& GetSrvHeap() { return cbvSrvUavHeap; }
	void UnloadAssets();
	// High Level
	HRenderTarget CreateRenderTarget();
	HDepthBuffer CreateDepthBuffer();
	void CreateMesh(HVertexBuffer vbIndexPosition, HVertexBuffer vbIndexColor, HVertexBuffer vbIndexTexture, const CHAR* name);
	void CreateTexture(UINT width, UINT height, BYTE* data, const CHAR* name);
	UINT CreateShaderResourceView(HTexture& textureHandle);
	void UploadTextureToBuffer(UINT width, UINT height, BYTE* data, HBuffer& bufferHandle);
	void FillTextureUploadBuffer(UINT width, UINT height, HBuffer& bufferHandle);
	void LoadTextureFromFile(UINT width, UINT height, HBuffer& bufferHandle);
	// Textures
	HTexture CreateEmptyTexture(UINT width, UINT height, const CHAR* name);
	HTexture CreateDepthTexture(UINT width, UINT height, const CHAR* name);
	HTexture CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name);
	void CopyTexture(HCommandList commandList, HTexture source, HTexture destination);
	HBuffer CreateTextureUploadBuffer(HTexture textureHandle);
	void CopyBufferToTexture(HCommandList commandList, HBuffer buffer, HTexture texture);
	void CreateDefaultSamplers();
	// Geometry
	HVertexBuffer CreateVertexBuffer(UINT numOfVertices, UINT numOfFloatsPerVertex, FLOAT* meshData, const CHAR* name);
	HVertexBuffer GenerateColors(float* data, size_t size, UINT numOfTriangles, const CHAR* name);
	Mesh* GetMesh(HMesh mesh) { return meshes[mesh.Index()]; }
	// Constants
	void SetInlineConstants(HCommandList commandList, UINT numOfConstants, void* data);
	// Binding
	void BindRenderTarget(HCommandList commandList, HRenderTarget renderTarget);
	void BindRenderTargetWithDepth(HCommandList commandList, HRenderTarget renderTarget, HDepthBuffer depthBuffer);
	void ResetCommandList(HCommandList commandList, HPipelineState pipelineState);
	void ResetCommandList(HCommandList commandList);
	void CloseCommandList(HCommandList commandList);
	void SetupRenderPass(HCommandList commandList, HPipelineState pipelineState, HRootSignature rootSignature, HViewportAndScissors viewportAndScissors);
	void SetDescriptorHeap(HCommandList commandList);
	void BindGeometry(HCommandList commandList, HMesh mesh);
	void BindTexture(HCommandList commandList, HTexture texture);
	// Clearing
	void CleraRenderTarget(HCommandList commandList, HRenderTarget renderTarget);
	void ClearDepthBuffer(HCommandList commandList, HDepthBuffer depthBuffer);
	// Barriers
	void TransitionTo(HCommandList commandList, HTexture texture, D3D12_RESOURCE_STATES state);
	void TransitionBack(HCommandList commandList, HTexture texture);
	void TransitionTo(HCommandList commandList, HBuffer buffer, D3D12_RESOURCE_STATES state);
	void TransitionBack(HCommandList commandList, HBuffer buffer);
	// Drawing
	void DrawMesh(HCommandList commandList, HMesh mesh);
private:
	DescriptorHeap rtvHeap;
	DescriptorHeap dsvHeap;
	DescriptorHeap cbvSrvUavHeap;
	DescriptorHeap samplerHeap;
	ID3D12Resource* backBuffer[2];
private:
	std::vector<RenderTarget*> renderTargets;
	std::vector<DepthBuffer*> depthBuffers;
	std::vector<CommandList*> commandLists;
	std::vector<Texture*> textures;
	std::vector<Buffer*> buffers;
	std::vector<VertexBuffer*> vertexBuffers;
	std::vector<Mesh*> meshes;
	std::vector<Material*> materials;
	std::vector<Shader*> shaders;
	std::vector<ID3D12RootSignature*> rootSignatures;
	std::vector<ID3D12PipelineState*> pipelineStates;
	std::vector<CD3DX12_VIEWPORT> viewports;
	std::vector<CD3DX12_RECT> scissorRects;
	std::vector<InputLayout*> inputLayouts;
};