#include "RenderContext.h"
#include "DeviceContext.h"
#include "WindowContext.h"
#include "RenderTarget.h"
#include "DepthBuffer.h"
#include "Mesh.h"
#include "VertexBuffer.h"
#include "InputLayout.h"

#include <Windows.h>
#include <d3dcompiler.h>
#include "../externals/SimpleMath/SimpleMath.h"
#include "debugapi.h"
#include "Utils.h"

extern WindowContext windowContext;
extern DeviceContext deviceContext;

RenderContext::RenderContext()
{
	OutputDebugString(L"RenderContext Constructor\n");
}

RenderContext::~RenderContext()
{
	OutputDebugString(L"RenderContext Destructor\n");

	// Reelease Render Resources
	for (auto& pipelineState : pipelineStates)
	{
		SafeRelease(&pipelineState);
	}
	for (auto& vertexShader : vertexShaders)
	{
		SafeRelease(&vertexShader);
	}
	for (auto& pixelShader : pixelShaders)
	{
		SafeRelease(&pixelShader);
	}
	for (auto& rootSignature : rootSignatures)
	{
		SafeRelease(&rootSignature);
	}
	
	for (int i = 0; i < FRAME_COUNT; i++)
	{
		SafeRelease(&backBuffer[i]);
	}
}

void RenderContext::CreateDescriptorHeap(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateDescriptorHeap\n");
	rtvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, deviceContext);
	dsvHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, deviceContext);
	cbvSrvUavHeap.Create(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV, deviceContext);
}

HRenderTarget RenderContext::CreateRenderTarget()
{
	OutputDebugString(L"CreateRenderTarget\n");

	HTexture texture = CreateRenderTargetTexture(windowContext.GetWidth(), windowContext.GetHeight(), "RT_Custom_Texture");
	deviceContext.GetDevice()->CreateRenderTargetView(textures[texture.Index()]->GetResource(), nullptr, rtvHeap.Allocate());

	renderTargets.push_back(new RenderTarget(1920, 1080, texture.Index(), rtvHeap.Size() - 1, "RT_Custom"));

	// We also need naming, for debugging purposes

	return HRenderTarget(renderTargets.size() - 1);
}

HDepthBuffer RenderContext::CreateDepthBuffer()
{
	OutputDebugString(L"CreateDepthBuffer\n");

	HTexture depth = CreateDepthTexture(windowContext.GetWidth(), windowContext.GetHeight(), "DB_Custom_Texture");
	deviceContext.GetDevice()->CreateDepthStencilView(textures[depth.Index()]->GetResource(), nullptr, dsvHeap.Allocate());

	depthBuffers.push_back(new DepthBuffer(windowContext.GetWidth(), windowContext.GetHeight(), depth.Index(), dsvHeap.Size() - 1, "DB_Custom"));

	return HDepthBuffer(depthBuffers.size() - 1);
}

void RenderContext::CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateRenderTargetFromBackBuffer\n");
	
	// Create a RTV for each frame
	for (UINT i = 0; i < FRAME_COUNT; i++)
	{
		// Here, we don't really create the underlying resource for the RTV
		// We just get the back buffer from the swap chain and create a RTV for it
		IDXGISwapChain* swapChain = deviceContext->GetSwapChain();
		ExitIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer[i])));
		backBuffer[i]->SetName(L"Render Context Back Buffer");
		CHAR name[] = "BackBuffer";
		textures.push_back(new Texture(windowContext.GetWidth(), windowContext.GetHeight(), backBuffer[i], &name[0]));

		deviceContext->GetDevice()->CreateRenderTargetView(backBuffer[i], nullptr, rtvHeap.Allocate());
		renderTargets.push_back(new RenderTarget(windowContext.GetWidth(), windowContext.GetHeight(), textures.size() - 1, rtvHeap.Size() - 1, "RT_BackBuffer"));
		OutputDebugString(L"CreateRenderTargetFromBackBuffer succeeded\n");
	}
}

HRootSignature RenderContext::CreateRootSignature(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateRootSignature\n");

	// Build Common Root Signature
	D3D12_ROOT_PARAMETER rootParameter;
	rootParameter.ParameterType = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
	rootParameter.Constants.Num32BitValues = 16;
	rootParameter.Constants.RegisterSpace = 0;
	rootParameter.Constants.ShaderRegister = 0;
	rootParameter.ShaderVisibility = D3D12_SHADER_VISIBILITY_ALL;

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(1, &rootParameter, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	ID3DBlob* error;
	ExitIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	ID3D12RootSignature* rootSignature;
	ExitIfFailed(deviceContext->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	rootSignature->SetName(L"Render Context Root Signature");
	
	rootSignatures.push_back(rootSignature);
	OutputDebugString(L"CreateRootSignature succeeded\n");
	return HRootSignature(rootSignatures.size() - 1);
}

HShader RenderContext::CreateShaders(LPCWSTR shaderName)
{
	OutputDebugString(L"CreateShaders\n");

#if defined(DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

	ID3DBlob* vertexShader;
	ID3DBlob* pixelShader;

	// In final we will copy shaders to the bin directory
	ExitIfFailed(D3DCompileFromFile(shaderName, nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
	ExitIfFailed(D3DCompileFromFile(shaderName, nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));

	vertexShaders.push_back(vertexShader);
	pixelShaders.push_back(pixelShader);
	OutputDebugString(L"CreateShaders succeeded\n");

	return HShader(vertexShaders.size() - 1);
}

HPipelineState RenderContext::CreatePipelineState(DeviceContext* deviceContext, HRootSignature rootSignature, HShader shader, HInputLayout inputLayout)
{
	OutputDebugString(L"CreatePipelineState\n");

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayouts[inputLayout.Index()]->GetInputLayoutDesc();
	psoDesc.pRootSignature = rootSignatures[rootSignature.Index()];
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShaders[shader.Index()]);
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShaders[shader.Index()]);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;
	ID3D12PipelineState* pipelineState;
	ExitIfFailed(deviceContext->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	pipelineState->SetName(L"Render Context Pipeline State");

	pipelineStates.push_back(pipelineState);

	OutputDebugString(L"CreatePipelineState succeeded\n");
	return HPipelineState(pipelineStates.size() - 1);
}

HViewportAndScissors RenderContext::CreateViewportAndScissorRect(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateViewportAndScissorRect\n");

	viewports.push_back(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(windowContext.GetWidth()), static_cast<float>(windowContext.GetHeight())));
	scissorRects.push_back(CD3DX12_RECT(0, 0, static_cast<LONG>(windowContext.GetWidth()), static_cast<LONG>(windowContext.GetHeight())));
	OutputDebugString(L"CreateViewportAndScissorRect succeeded\n");

	return HViewportAndScissors(viewports.size() - 1);
}

HInputLayout RenderContext::CreateInputLayout()
{
	OutputDebugString(L"CreateInputLayout\n");
	inputLayouts.push_back(new InputLayout());
	return HInputLayout(inputLayouts.size() - 1);
}

HVertexBuffer RenderContext::CreateVertexBuffer(UINT numOfVertices, UINT numOfFloatsPerVertex, FLOAT* meshData, const CHAR* name)
{
	OutputDebugString(L"CreateVertexBuffer\n");
	
	// Each vertex is: 4xFLOAT for position + 4xFLOAT for color
	const UINT vbSizeInBytes = numOfVertices * numOfFloatsPerVertex * sizeof(float);

	// Note: using upload heaps to transfer static data like vert buffers is not 
	// recommended. Every time the GPU needs it, the upload heap will be marshalled 
	// over. Please read up on Default Heap usage. An upload heap is used here for 
	// code simplicity and because there are very few verts to actually transfer.
	auto heapProperties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(vbSizeInBytes);
	const D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_COMMON;
	ID3D12Resource* vertexBuffer;
	D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;
	deviceContext.CreateUploadResource(heapFlags, &resourceDesc, initResourceState, IID_PPV_ARGS(& vertexBuffer));

	CHAR tempName[64];
	snprintf(tempName, sizeof(tempName), "VB_%s", name);
	WCHAR wName[64];
	size_t numOfCharsConverted;;
	mbstowcs_s(&numOfCharsConverted, wName, tempName, 32);
	vertexBuffer->SetName(wName);

	vertexBuffers.push_back(new VertexBuffer(vertexBuffer, vbSizeInBytes, numOfVertices, tempName));
	
	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	
	const auto index = vertexBuffers.size() - 1;
	auto vb = vertexBuffers[index]->GetResource();
	ExitIfFailed(vb->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, meshData, vbSizeInBytes);
	vb->Unmap(0, nullptr);

	return HVertexBuffer(index);
}

HVertexBuffer RenderContext::GenerateColors(float* data, size_t size, UINT numOfTriangles, const CHAR* name)
{
#define COLOR_1 153.0f / 255.0f, 202.0f / 255.0f, 34.0f / 255.0f, 1.0f
#define COLOR_2 160.0f / 255.0f, 210.0f / 255.0f, 31.0f / 255.0f, 1.0f
#define COLOR_3 173.0f / 255.0f, 223.0f / 255.0f, 44.0f / 255.0f, 1.0f

#define COLOR_4 161.0f / 255.0f, 92.0f / 255.0f, 208.0f / 255.0f, 1.0f
#define COLOR_5 166.0f / 255.0f, 96.0f / 255.0f, 213.0f / 255.0f, 1.0f
#define COLOR_6 179.0f / 255.0f, 110.0f / 255.0f, 227.0f / 255.0f, 1.0f

#define COLOR_7 191.0f / 255.0f, 111.0f / 255.0f, 29.0f / 255.0f, 1.0f
#define COLOR_8 202.0f / 255.0f, 122.0f / 255.0f, 37.0f / 255.0f, 1.0f
#define COLOR_9 225.0f / 255.0f, 152.0f / 255.0f, 58.0f / 255.0f, 1.0f

#define COLOR_10 17.0f / 255.0f, 85.0f / 255.0f, 60.0f / 255.0f, 1.0f
#define COLOR_11 39.0f / 255.0f, 112.0f / 255.0f, 85.0f / 255.0f, 1.0f
#define COLOR_12 85.0f / 255.0f, 154.0f / 255.0f, 119.0f / 255.0f, 1.0f

#define COLOR_13 86.0f / 255.0f, 126.0f / 255.0f, 145.0f / 255.0f, 1.0f
#define COLOR_14 112.0f / 255.0f, 153.0f / 255.0f, 172.0f / 255.0f, 1.0f
#define COLOR_15 138.0f / 255.0f, 180.0f / 255.0f, 200.0f / 255.0f, 1.0f

#define COLOR_16 162.0f / 255.0f, 208.0f / 255.0f, 181.0f / 255.0f, 1.0f
#define COLOR_17 170.0f / 255.0f, 214.0f / 255.0f, 188.0f / 255.0f, 1.0f
#define COLOR_18 179.0f / 255.0f, 220.0f / 255.0f, 195.0f / 255.0f, 1.0f

#define COLOR_19 24.0f / 255.0f, 207.0f / 255.0f, 45.0f / 255.0f, 1.0f
#define COLOR_20 37.0f / 255.0f, 219.0f / 255.0f, 55.0f / 255.0f, 1.0f
#define COLOR_21 51.0f / 255.0f, 231.0f / 255.0f, 66.0f / 255.0f, 1.0f

#define COLOR_22 199.0f / 255.0f, 53.0f / 255.0f, 52.0f / 255.0f, 1.0f
#define COLOR_23 208.0f / 255.0f, 64.0f / 255.0f, 62.0f / 255.0f, 1.0f
#define COLOR_24 218.0f / 255.0f, 75.0f / 255.0f, 73.0f / 255.0f, 1.0f

	// We need a buffer of the same size as the original mesh
	float* meshPositionAndColor = new float[size];
	float color[] = { COLOR_1, COLOR_2, COLOR_3,
		COLOR_4, COLOR_5, COLOR_6,
		COLOR_7, COLOR_8, COLOR_9,
		COLOR_10, COLOR_11, COLOR_12,
		COLOR_13, COLOR_14, COLOR_15,
		COLOR_16, COLOR_17, COLOR_18,
		COLOR_19, COLOR_20, COLOR_21,
		COLOR_22, COLOR_23, COLOR_24
	};
	// Currently we handle seven colors, each color has 12 "elements" (three positions of four floats)
	size_t colorOffset = (meshes.size() % 8) * 12;
	for (int i = 0; i < size; i += 4)
	{
		// This loop copies vertices (not triangles) and adds color to them
		// We don't need to copy the position, we just need to add the color
		//memcpy(&meshPositionAndColor[i * 2], &data[i], 4 * sizeof(FLOAT));
		const unsigned int colorIndex = (i / 12) % 3;
		memcpy(&meshPositionAndColor[i], &color[(colorIndex * 4) + colorOffset], 4 * sizeof(FLOAT));
	}

	CHAR tempName[64];
	snprintf(tempName, sizeof(tempName), "COLOR_%s", name);

	HVertexBuffer vertexBuffer = CreateVertexBuffer(numOfTriangles * 3, 4, meshPositionAndColor, tempName);

	return vertexBuffer;
}

HTexture RenderContext::CreateEmptyTexture(UINT width, UINT height)
{
	OutputDebugString(L"CreateEmptyTexture\n");

	D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
		width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_NONE);

	D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_COMMON;

	ID3D12Resource* resource;
	deviceContext.CreateResource(heapFlags, &desc, initResourceState, IID_PPV_ARGS(&resource));
	resource->SetName(L"Empty Texture");
	
	CHAR name[] = "EmptyTexture";
	textures.push_back(new Texture(width, height, resource, &name[0]));
	
	return HTexture(textures.size() - 1);
}

HTexture RenderContext::CreateDepthTexture(UINT width, UINT height, const CHAR* name)
{
	OutputDebugString(L"CreateDepthTexture\n");
	D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT,
		width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);

	D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_DEPTH_WRITE;

	ID3D12Resource* resource;
	deviceContext.CreateResource(heapFlags, &desc, initResourceState, IID_PPV_ARGS(&resource));
	resource->SetName(L"Depth Texture");

	CHAR tempName[32];
	strcpy_s(tempName, name);
	WCHAR wName[32];
	size_t numOfCharsConverted;;
	mbstowcs_s(&numOfCharsConverted, wName, tempName, 32);
	resource->SetName(wName);
	textures.push_back(new Texture(width, height, resource, &tempName[0]));

	return HTexture(textures.size() - 1);
}

HTexture RenderContext::CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name)
{
	OutputDebugString(L"CreateRenderTargetTexture\n");

	D3D12_HEAP_FLAGS heapFlags = D3D12_HEAP_FLAG_NONE;

	CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_R8G8B8A8_UNORM,
		width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET);

	D3D12_RESOURCE_STATES initResourceState = D3D12_RESOURCE_STATE_RENDER_TARGET;

	ID3D12Resource* resource;
	deviceContext.CreateResource(heapFlags, &desc, initResourceState, IID_PPV_ARGS(&resource));

	CHAR tempName[32];
	strcpy_s(tempName, name);
	WCHAR wName[32];
	size_t numOfCharsConverted;;
	mbstowcs_s(&numOfCharsConverted, wName, tempName, 32);
	resource->SetName(wName);
	textures.push_back(new Texture(width, height, resource, &tempName[0], initResourceState));

	return HTexture(textures.size() - 1);
}

void RenderContext::CopyTexture(HCommandList commandList, HTexture source, HTexture destination)
{
	D3D12_TEXTURE_COPY_LOCATION destLocation = {};
	destLocation.pResource = textures[destination.Index()]->GetResource();
	destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destLocation.SubresourceIndex = 0;

	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = textures[source.Index()]->GetResource();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	srcLocation.SubresourceIndex = 0;
	commandLists[commandList.Index()]->GetCommandList()->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);
}

void RenderContext::CreateMesh(HVertexBuffer vbIndexPosition, HVertexBuffer vbIndexColor, const CHAR* name)
{
	OutputDebugString(L"CreateMesh\n");

	auto createVBV = [&](HVertexBuffer handle, UINT stride) -> D3D12_VERTEX_BUFFER_VIEW
	{
		auto& vb = vertexBuffers[handle.Index()];
		D3D12_VERTEX_BUFFER_VIEW vbv;
		vbv.BufferLocation = vb->GetResource()->GetGPUVirtualAddress();
		vbv.StrideInBytes = stride;
		vbv.SizeInBytes = vb->GetSizeInBytes();
		return vbv;
	};

	D3D12_VERTEX_BUFFER_VIEW vbvPosition = createVBV(vbIndexPosition, 4 * sizeof(float));
	D3D12_VERTEX_BUFFER_VIEW vbvColor = createVBV(vbIndexColor, 4 * sizeof(float));

	UINT vertexCount = vertexBuffers[vbIndexPosition.Index()]->GetNumOfVertices();
	meshes.push_back(new Mesh(vbIndexPosition.Index(), vbvPosition, vbIndexColor.Index(), vbvColor, vertexCount, name));
}


void RenderContext::SetInlineConstants(HCommandList commandList, UINT numOfConstants, void* data)
{
	commandLists[commandList.Index()]->GetCommandList()->SetGraphicsRoot32BitConstants(0, numOfConstants, data, 0);
}

void RenderContext::BindRenderTarget(HCommandList commandList, HRenderTarget renderTarget)
{
	auto rtvHandleIndex = renderTargets[renderTarget.Index()]->GetDescriptorIndex();
	auto rtvHandle = rtvHeap.Get(rtvHandleIndex);
	commandLists[commandList.Index()]->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void RenderContext::BindRenderTargetWithDepth(HCommandList commandList, HRenderTarget renderTarget, HDepthBuffer depthBuffer)
{
	auto rtvHandleIndex = renderTargets[renderTarget.Index()]->GetDescriptorIndex();
	auto rtvHandle = rtvHeap.Get(rtvHandleIndex);
	auto dsvHandleIndex = depthBuffers[depthBuffer.Index()]->GetDescriptorIndex();
	auto dsvHandle = dsvHeap.Get(dsvHandleIndex);
	commandLists[commandList.Index()]->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void RenderContext::CleraRenderTarget(HCommandList commandList, HRenderTarget renderTarget)
{
	auto rtvHandleIndex = renderTargets[renderTarget.Index()]->GetDescriptorIndex();
	auto rtvHandle = rtvHeap.Get(rtvHandleIndex);
	float clearColor[] = { 1.000f, 0.980f, 0.900f, 1.0f };
	commandLists[commandList.Index()]->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void RenderContext::ClearDepthBuffer(HCommandList commandList, HDepthBuffer depthBuffer)
{
	auto dsvHandleIndex = depthBuffers[depthBuffer.Index()]->GetDescriptorIndex();
	auto dsvHandle = dsvHeap.Get(dsvHandleIndex);
	commandLists[commandList.Index()]->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderContext::ResetCommandList(HCommandList commandList, HPipelineState pipelineState)
{
	commandLists[commandList.Index()]->Reset(pipelineStates[pipelineState.Index()]);
}

void RenderContext::ResetCommandList(HCommandList commandList)
{
	commandLists[commandList.Index()]->Reset(nullptr);
}

void RenderContext::CloseCommandList(HCommandList commandList)
{
	commandLists[commandList.Index()]->Close();
}

void RenderContext::SetupRenderPass(HCommandList commandList, HPipelineState pipelineState, HRootSignature rootSignature, HViewportAndScissors viewportAndScissors)
{
	commandLists[commandList.Index()]->GetCommandList()->SetGraphicsRootSignature(rootSignatures[rootSignature.Index()]);
	commandLists[commandList.Index()]->GetCommandList()->SetPipelineState(pipelineStates[pipelineState.Index()]);
	commandLists[commandList.Index()]->GetCommandList()->RSSetViewports(1, &viewports[viewportAndScissors.Index()]);
	commandLists[commandList.Index()]->GetCommandList()->RSSetScissorRects(1, &scissorRects[viewportAndScissors.Index()]);
}

void RenderContext::BindGeometry(HCommandList commandList, HMesh mesh)
{
	commandLists[commandList.Index()]->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	D3D12_VERTEX_BUFFER_VIEW vbvPosition[] =
	{
		meshes[mesh.Index()]->GetPositionVertexBufferView(),
		meshes[mesh.Index()]->GetColorVertexBufferView()
	};
	commandLists[commandList.Index()]->GetCommandList()->IASetVertexBuffers(0, 2, vbvPosition);
}

void RenderContext::TransitionTo(HCommandList commandList, HTexture texture, D3D12_RESOURCE_STATES state)
{
	if (textures[texture.Index()]->GetCurrentState() == state)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier;
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textures[texture.Index()]->GetResource();
	barrier.Transition.StateBefore = textures[texture.Index()]->GetCurrentState();
	barrier.Transition.StateAfter = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandLists[commandList.Index()]->GetCommandList()->ResourceBarrier(1, &barrier);

	textures[texture.Index()]->SetCurrentState(state);
}

void RenderContext::TransitionBack(HCommandList commandList, HTexture texture)
{
	auto previousState = textures[texture.Index()]->GetPreviousState();
	TransitionTo(commandList, texture, previousState);
}

void RenderContext::DrawMesh(HCommandList commandList, HMesh mesh)
{
	UINT vertexCount = meshes[mesh.Index()]->GetVertexCount();
	commandLists[commandList.Index()]->GetCommandList()->DrawInstanced(vertexCount, 1, 0, 0);
}

ID3D12Resource* RenderContext::GetCurrentBackBuffer()
{
	auto frameIndex = deviceContext.GetCurrentBackBufferIndex();
	return backBuffer[frameIndex];
}

void RenderContext::ExecuteCommandList(HCommandList commandList)
{
	ID3D12CommandList* ppCommandLists[] = { commandLists[commandList.Index()]->GetCommandList()};
	deviceContext.GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

HCommandList RenderContext::CreateCommandList()
{
	size_t index = commandLists.size();
	commandLists.push_back(new CommandList());
	commandLists[index]->Create();

	return HCommandList(index);
}
