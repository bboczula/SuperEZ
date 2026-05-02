#pragma once

#include <vector>
#include "../bind/DescriptorHeap.h" // It includes d3d12.h
#include "../asset/Handle.h"
#include "RenderItem.h"
#include "../bind/CommandList.h"

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
class Camera;
class RootSignatureBuilder;

enum RenderTargetFormat
{
	RGB8_UNORM,
	R32G32_UINT,
	R32_UINT
};

enum class SceneEntityKind : unsigned char
{
	Renderable,
	Camera,
	Sunlight
};

struct SceneEntityRecord
{
	uint32_t id = UINT32_MAX;
	SceneEntityKind kind = SceneEntityKind::Renderable;
	char name[32] = {};
	HMesh mesh;
	HTexture texture;
	UINT cameraIndex = UINT32_MAX;
};

struct SunlightConstants
{
	float lightDirection[4] = { -0.4f, -1.0f, -0.3f, 0.0f };
	float lightColor[4] = { 1.0f, 0.98f, 0.92f, 0.0f };
	float ambientStrength = 0.2f;
	float diffuseStrength = 1.0f;
};

struct SunlightViewProjection
{
	DirectX::SimpleMath::Matrix viewProjection = DirectX::SimpleMath::Matrix::Identity;
};

class RenderContext
{
public:
	RenderContext();
	~RenderContext();
	void CreateDescriptorHeap(DeviceContext* deviceContext);
	void CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext);
	HRootSignature CreateRootSignature(RootSignatureBuilder& builder);
	HShader CreateShader(LPCWSTR shaderFileName, LPCSTR entryPoint, LPCSTR shaderModel);
	HPipelineState CreatePipelineState(DeviceContext* deviceContext, HRootSignature rootSignature, HShader vertexShader,
		HShader pixelShader, HInputLayout inputLayout, HRenderTarget renderTarget);
	HPipelineState CreatePipelineState(DeviceContext* deviceContext, HRootSignature rootSignature, HShader computeShader);
	HInputLayout CreateInputLayout();
	InputLayout* GetInputLayout(HInputLayout inputLayout) { return inputLayouts[inputLayout.Index()]; }
	void CreateIndexBuffer(DeviceContext* deviceContext);
	void CreateConstantBuffer(DeviceContext* deviceContext);
	void CreateSampler(DeviceContext* deviceContext);
	void CreateShader(DeviceContext* deviceContext);
	void ExecuteCommandList(HCommandList commandList);
	HCommandList CreateCommandList();
	CommandList* GetCommandList(HCommandList commandList) { return commandLists[commandList.Index()]; }
	UINT GetNumOfMeshes() { return static_cast<UINT>(meshes.size()); }
	DescriptorHeap& GetSrvHeap() { return cbvSrvUavHeap; }
	void UnloadAssets();
	// High Level
	std::vector<RenderItem>& GetRenderItems();
	RenderItem* GetRenderItemById(uint32_t id);
	const std::vector<SceneEntityRecord>& GetSceneEntities() const { return sceneEntities; }
	SceneEntityRecord* GetSceneEntityById(uint32_t id);
	void RegisterRenderableEntity(uint32_t id, const char* name, HMesh mesh, HTexture texture);
	void RegisterCameraEntity(uint32_t id, const char* name, UINT cameraIndex);
	void RegisterSunlightEntity(uint32_t id, const char* name);
	const SunlightConstants& GetSunlightConstants() const { return sunlightConstants; }
	void SetSunlightConstants(const SunlightConstants& constants) { sunlightConstants = constants; }
	void UpdateSunlightViewProjection();
	const SunlightViewProjection& GetSunlightViewProjection() const { return sunlightViewProjection; }
	void CreateRenderItem(const RenderItem& item);
	HRenderTarget CreateRenderTarget(const char* name, RenderTargetFormat format);
	HRenderTarget CreateRenderTarget(const char* name, RenderTargetFormat format, int width, int height);
	HRenderTarget CreateRenderTarget(const char* name, HTexture texture);
	HDepthBuffer CreateDepthBuffer();
	HDepthBuffer CreateDepthBuffer(UINT width, UINT height, const char* name);
	void CreateMesh(HVertexBuffer vbIndexPosition, HVertexBuffer vbIndexColor, HVertexBuffer vbIndexTexture, HVertexBuffer vbNormalsTexture, const CHAR* name);
	void CreateTexture(UINT width, UINT height, BYTE* data, const CHAR* name);
	UINT CreateUnorderedAccessView(ID3D12Resource* resource, DXGI_FORMAT format, bool isStatic);
	UINT CreateCamera(float aspectRatio, DirectX::SimpleMath::Vector3 position, DirectX::SimpleMath::Vector3 rotation);
	Camera* GetCamera(UINT index) { return cameras[index]; }
	Camera* GetActiveCamera() { return cameras[activeCameraIndex]; }
	UINT GetActiveCameraIndex() const { return activeCameraIndex; }
	void SetActiveCamera(UINT index) { activeCameraIndex = index; }
	HTexture GetTexture(HRenderTarget renderTarget);
	HTexture GetTexture(HDepthBuffer depthBuffer);
	HTexture GetTexture(const char* name);
	std::vector<uint8_t> ReadbackBufferData(HBuffer handle, size_t size);
	void SetSelectedObjectId(uint32_t id) { currentSelectedObjectID = id; }
	bool WasObjectSelected() { return wasObjectSeleced; }
	void SetWasObjectSelected(bool value) { wasObjectSeleced = value; }
	uint32_t GetSelectedObjectId() const { return currentSelectedObjectID; }
	RenderTarget* GetRenderTarget(HRenderTarget renderTarget) { return renderTargets[renderTarget.Index()]; }
	// Textures
	HTexture CreateEmptyTexture(UINT width, UINT height, DXGI_FORMAT format, const CHAR* name, bool isUav = false);
	HTexture CreateDepthTexture(UINT width, UINT height, const CHAR* name);
	HTexture CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name, DXGI_FORMAT format);
	void CopyTexture(HCommandList commandList, HTexture source, HTexture destination);
	void CopyBufferToTexture(HCommandList commandList, HBuffer buffer, HTexture texture);
	void CopyTextureToBuffer(HCommandList commandList, HTexture texture, HBuffer buffer, LONG mouseX, LONG mouseY);
	void CreateDefaultSamplers();
	UINT CreateShaderResourceView(HTexture& textureHandle);
	UINT CreateShaderResourceView(ID3D12Resource* resource, DXGI_FORMAT format, bool isStatic);
	void UploadTextureToBuffer(UINT width, UINT height, BYTE* data, HBuffer& bufferHandle);
	void FillTextureUploadBuffer(UINT width, UINT height, HBuffer& bufferHandle);
	void LoadTextureFromFile(UINT width, UINT height, HBuffer& bufferHandle);
	Texture* GetTexture(HTexture texture) { return textures[texture.Index()]; }
	// Buffers
	HBuffer CreateReadbackBuffer();
	template<typename T>
	HBuffer CreateConsantBuffer();
	HBuffer CreateTextureUploadBuffer(HTexture textureHandle);
	// Geometry
	HVertexBuffer CreateVertexBuffer(UINT numOfVertices, UINT numOfFloatsPerVertex, FLOAT* meshData, const CHAR* name);
	HVertexBuffer GenerateColors(float* data, size_t size, UINT numOfTriangles, const CHAR* name);
	Mesh* GetMesh(HMesh mesh) { return meshes[mesh.Index()]; }
	// Constants
	// Non-template, implemented in .cpp (can include CommandList.h there)
	void SetInlineConstantsRaw(HCommandList commandList,
		UINT num32BitValues,
		const void* data,
		UINT slot) const;

	template<typename T>
	void SetInlineConstants(HCommandList commandList, const T& data, UINT slot) const
	{
		static_assert((sizeof(T) % 4) == 0, "Root constants must be 32-bit aligned.");
		SetInlineConstantsRaw(commandList, (UINT)(sizeof(T) / 4), &data, slot);
	}
	void SetInlineConstantsUAV(HCommandList commandList, UINT numOfConstants, void* data, UINT slot);
	// Binding
	void BindRenderTarget(HCommandList commandList, HRenderTarget renderTarget);
	void BindRenderTargetWithDepth(HCommandList commandList, HRenderTarget renderTarget, HDepthBuffer depthBuffer);
	void ResetCommandList(HCommandList commandList, HPipelineState pipelineState);
	void ResetCommandList(HCommandList commandList);
	void CloseCommandList(HCommandList commandList);
	void SetupRenderPass(HCommandList commandList, HPipelineState pipelineState, HRootSignature rootSignature);
	void SetDescriptorHeap(HCommandList commandList);
	void SetDescriptorHeapCompute(HCommandList commandList);
	void BindGeometry(HCommandList commandList, HMesh mesh);
	void BindConstantBuffer(HCommandList commandList, HBuffer buffer, UINT slot);
	void UpdateConstantBuffer(HBuffer buffer, const void* data, UINT sizeInBytes);
	void BindTexture(HCommandList commandList, HTexture texture, UINT slot);
	void BindTextureOnlyUAV(HCommandList commandList, HTexture texture, UINT slot);
	void BindTextureOnlySRV(HCommandList commandList, HTexture texture, UINT slot);
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
	void Dispatch(HCommandList commandList, UINT threadGroupX, UINT threadGroupY, UINT threadGroupZ);
	// A huuuuge hack...
	std::vector<RenderItem> renderItems;
private:
	HBuffer CreateConstantBufferInternal(UINT bufferSizeInBytes);
private:
	DescriptorHeap rtvHeap;
	DescriptorHeap dsvHeap;
	DescriptorHeap cbvSrvUavHeap;
	DescriptorHeap samplerHeap;
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
	std::vector<RootSignature*> rootSignatures;
	std::vector<PipelineState*> pipelineStates;
	std::vector<InputLayout*> inputLayouts;
	std::vector<Camera*> cameras;
	std::vector<SceneEntityRecord> sceneEntities;
	SunlightConstants sunlightConstants;
	SunlightViewProjection sunlightViewProjection;
private:
	uint32_t currentSelectedObjectID = ~0u; // ~0u == invalid ID (aka nothing selected)
	bool wasObjectSeleced = false;
	UINT activeCameraIndex = 0;
};

template<typename T>
inline HBuffer RenderContext::CreateConsantBuffer()
{
	// Round size up to 256
	UINT cbSize = (sizeof(T) + 255) & ~255;

	return CreateConstantBufferInternal(cbSize);
}
