#include "RenderContext.h"
#include "DeviceContext.h"
#include "WindowContext.h"
#include "RenderTarget.h"
#include "DepthBuffer.h"

#include <Windows.h>
#include <d3dcompiler.h>
#include "../externals/SimpleMath/SimpleMath.h"
#include "../externals/AssetSuite/inc/AssetSuite.h"
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

size_t RenderContext::CreateRenderTarget()
{
	OutputDebugString(L"CreateRenderTarget\n");

	size_t textureIndex = CreateRenderTargetTexture(windowContext.GetWidth(), windowContext.GetHeight(), "RT_Custom_Texture");
	deviceContext.GetDevice()->CreateRenderTargetView(textures[textureIndex]->GetResource(), nullptr, rtvHeap.Allocate());

	renderTargets.push_back(new RenderTarget(1920, 1080, textureIndex, rtvHeap.Size() - 1, "RT_Custom"));

	// We also need naming, for debugging purposes

	return renderTargets.size() - 1;
}

size_t RenderContext::CreateDepthBuffer()
{
	OutputDebugString(L"CreateDepthBuffer\n");

	size_t depthIndex = CreateDepthTexture(windowContext.GetWidth(), windowContext.GetHeight(), "DB_Custom_Texture");
	deviceContext.GetDevice()->CreateDepthStencilView(textures[depthIndex]->GetResource(), nullptr, dsvHeap.Allocate());

	depthBuffers.push_back(new DepthBuffer(windowContext.GetWidth(), windowContext.GetHeight(), depthIndex, dsvHeap.Size() - 1, "DB_Custom"));

	return depthBuffers.size() - 1;
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

size_t RenderContext::CreateRootSignature(DeviceContext* deviceContext)
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
	return rootSignatures.size() - 1;
}

size_t RenderContext::CreateShaders(LPCWSTR shaderName)
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

	return vertexShaders.size() - 1;
}

size_t RenderContext::CreatePipelineState(DeviceContext* deviceContext, size_t rootSignatureIndex, size_t shaderIndex)
{
	OutputDebugString(L"CreatePipelineState\n");

	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
	    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
	    { "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 16, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignatures[rootSignatureIndex];
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShaders[shaderIndex]);
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShaders[shaderIndex]);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	//psoDesc.DepthStencilState.DepthEnable = FALSE;
	//psoDesc.DepthStencilState.StencilEnable = FALSE;
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
	return pipelineStates.size() - 1;
}

size_t RenderContext::CreateViewportAndScissorRect(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateViewportAndScissorRect\n");

	viewports.push_back(CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(windowContext.GetWidth()), static_cast<float>(windowContext.GetHeight())));
	scissorRects.push_back(CD3DX12_RECT(0, 0, static_cast<LONG>(windowContext.GetWidth()), static_cast<LONG>(windowContext.GetHeight())));
	OutputDebugString(L"CreateViewportAndScissorRect succeeded\n");

	return viewports.size() - 1;
}

void RenderContext::CreateVertexBuffer(DeviceContext* deviceContext)
{
	float ratio = static_cast<float>(windowContext.GetWidth()) / static_cast<float>(windowContext.GetHeight());

#define COLOR_1 0.890f, 0.430f, 0.070f, 1.0f
#define COLOR_2 0.816f, 0.324f, 0.070f, 1.0f
#define COLOR_3 0.972f, 0.632f, 0.214f, 1.0f

	std::filesystem::path currentPath = std::filesystem::current_path();
	currentPath.append("monkey.obj");

	AssetSuite::Manager assetManager;
	assetManager.MeshLoadAndDecode(currentPath.string().c_str(), AssetSuite::MeshDecoders::WAVEFRONT);

	std::vector<FLOAT> meshOutput;
	AssetSuite::MeshDescriptor meshDescriptor;
	auto errorCode = assetManager.MeshGet("Suzanne_Mesh", AssetSuite::MeshOutputFormat::POSITION, meshOutput, meshDescriptor);

	// Basically, each vertex has 4 floats, and we need to add 4 more for the color
	float* meshPositionAndColor = new float[meshOutput.size() * 2];
	const float color[] = { COLOR_1, COLOR_2, COLOR_3 };
	for (int i = 0; i < meshOutput.size(); i+= 4)
	{
		// This loop copies vertices (not triangles) and adds color to them
		memcpy(&meshPositionAndColor[i * 2], &meshOutput[i], 4 * sizeof(FLOAT));
		const unsigned int colorIndex = (i / 12) % 3;
		memcpy(&meshPositionAndColor[i * 2 + 4], &color[colorIndex * 4], 4 * sizeof(FLOAT));
	}
	
	const UINT vbSizeInBytes = meshOutput.size() * 2 * sizeof(float);

	auto buffer = deviceContext->CreateVertexBuffer(vbSizeInBytes);
	vertexBuffers.push_back(buffer);
	
	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	
	auto vb = vertexBuffers[0];
	ExitIfFailed(vb->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, meshPositionAndColor, vbSizeInBytes);
	vb->Unmap(0, nullptr);
	
	// Initialize the vertex buffer view.
	vertexBufferView.BufferLocation = vb->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = 8 * sizeof(float);
	vertexBufferView.SizeInBytes = vbSizeInBytes;
}

size_t RenderContext::CreateEmptyTexture(UINT width, UINT height)
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
	
	return textures.size() - 1;
}

size_t RenderContext::CreateDepthTexture(UINT width, UINT height, const CHAR* name)
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

	return textures.size() - 1;
}

size_t RenderContext::CreateRenderTargetTexture(UINT width, UINT height, const CHAR* name)
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

	return textures.size() - 1;
}

UINT RenderContext::CopyTexture(size_t cmdListIndex, size_t sourceIndex, size_t destIndex)
{
	// Here you should copy the Render Target from the previous Render Context and copy it to the Back Buffer
	// commandList->CopyTextureRegion();
	D3D12_TEXTURE_COPY_LOCATION destLocation = {};
	destLocation.pResource = textures[destIndex]->GetResource(); //destination->GetResource();
	destLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	destLocation.SubresourceIndex = 0; // Not sure if this is correct;

	D3D12_TEXTURE_COPY_LOCATION srcLocation = {};
	srcLocation.pResource = textures[sourceIndex]->GetResource(); //source->GetResource();
	srcLocation.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
	srcLocation.SubresourceIndex = 0;
	commandLists[cmdListIndex]->GetCommandList()->CopyTextureRegion(&destLocation, 0, 0, 0, &srcLocation, nullptr);

	return 0;
}

void RenderContext::SetInlineConstants(size_t cmdListIndex, UINT numOfConstants, void* data)
{
	commandLists[cmdListIndex]->GetCommandList()->SetGraphicsRoot32BitConstants(0, numOfConstants, data, 0);
}

void RenderContext::BindRenderTarget(size_t cmdListIndex, size_t rtIndex)
{
	auto rtvHandleIndex = renderTargets[rtIndex]->GetDescriptorIndex();
	auto rtvHandle = rtvHeap.Get(rtvHandleIndex);
	commandLists[cmdListIndex]->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
}

void RenderContext::BindRenderTargetWithDepth(size_t cmdListIndex, size_t rtIndex, size_t depthIndex)
{
	auto rtvHandleIndex = renderTargets[rtIndex]->GetDescriptorIndex();
	auto rtvHandle = rtvHeap.Get(rtvHandleIndex);
	auto dsvHandleIndex = depthBuffers[depthIndex]->GetDescriptorIndex();
	auto dsvHandle = dsvHeap.Get(dsvHandleIndex);
	commandLists[cmdListIndex]->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvHandle);
}

void RenderContext::CleraRenderTarget(size_t cmdListIndex, size_t rtIndex)
{
	auto rtvHandleIndex = renderTargets[rtIndex]->GetDescriptorIndex();
	auto rtvHandle = rtvHeap.Get(rtvHandleIndex);
	float clearColor[] = { 1.000f, 0.980f, 0.900f, 1.0f };
	commandLists[cmdListIndex]->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}

void RenderContext::ClearDepthBuffer(size_t cmdListIndex, size_t depthIndex)
{
	auto dsvHandleIndex = depthBuffers[depthIndex]->GetDescriptorIndex();
	auto dsvHandle = dsvHeap.Get(dsvHandleIndex);
	commandLists[cmdListIndex]->GetCommandList()->ClearDepthStencilView(dsvHandle, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void RenderContext::ResetCommandList(size_t cmdListindex, size_t psoIndex)
{
	commandLists[cmdListindex]->Reset(pipelineStates[psoIndex]);
}

void RenderContext::ResetCommandList(size_t index)
{
	commandLists[index]->Reset(nullptr);
}

void RenderContext::CloseCommandList(size_t index)
{
	commandLists[index]->Close();
}

void RenderContext::SetupRenderPass(size_t cmdListIndex, size_t psoIndex, size_t rootSignatureIndex, size_t viewportIndex, size_t scissorsIndex)
{
	commandLists[cmdListIndex]->GetCommandList()->SetGraphicsRootSignature(rootSignatures[rootSignatureIndex]);
	commandLists[cmdListIndex]->GetCommandList()->SetPipelineState(pipelineStates[psoIndex]);
	commandLists[cmdListIndex]->GetCommandList()->RSSetViewports(1, &viewports[viewportIndex]);
	commandLists[cmdListIndex]->GetCommandList()->RSSetScissorRects(1, &scissorRects[scissorsIndex]);
}

void RenderContext::BindGeometry(size_t cmdListIndex)
{
	commandLists[cmdListIndex]->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandLists[cmdListIndex]->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
}

void RenderContext::TransitionTo(size_t cmdListIndex, size_t textureId, D3D12_RESOURCE_STATES state)
{
	if (textures[textureId]->GetCurrentState() == state)
	{
		return;
	}

	D3D12_RESOURCE_BARRIER barrier;
	ZeroMemory(&barrier, sizeof(barrier));
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = textures[textureId]->GetResource();
	barrier.Transition.StateBefore = textures[textureId]->GetCurrentState();
	barrier.Transition.StateAfter = state;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	commandLists[cmdListIndex]->GetCommandList()->ResourceBarrier(1, &barrier);

	textures[textureId]->SetCurrentState(state);
}

void RenderContext::TransitionBack(size_t cmdListIndex, size_t textureId)
{
	auto previousState = textures[textureId]->GetPreviousState();
	TransitionTo(cmdListIndex, textureId, previousState);
}

ID3D12Resource* RenderContext::GetCurrentBackBuffer()
{
	auto frameIndex = deviceContext.GetCurrentBackBufferIndex();
	return backBuffer[frameIndex];
}

void RenderContext::PopulateCommandList(DeviceContext* deviceContext)
{
	// This is common to all Render Passes
	commandLists[0]->Reset(pipelineStates[0]);
	

	// This is per-pass stuff
	commandLists[0]->GetCommandList()->SetGraphicsRootSignature(rootSignatures[0]);
	commandLists[0]->GetCommandList()->RSSetViewports(1, &viewports[0]);
	commandLists[0]->GetCommandList()->RSSetScissorRects(1, &scissorRects[0]);

	// We could have a separate Render Target for each pass, and then finally a blit from last (or any) Render Pass

	// This is blit to back buffer actually
	auto barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandLists[0]->GetCommandList()->ResourceBarrier(1, &barrierToRenderTarget);
	
	auto rtvHandle = rtvHeap.Get(deviceContext->GetCurrentBackBufferIndex());
	commandLists[0]->GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	float clearColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	commandLists[0]->GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	// This is per-pass I think, though might not necessarily be
	// By design, I have only two types of passes, a fullscreen pass and pass that has access to game object tree
	// Maybe this could be somehow automated
	commandLists[0]->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandLists[0]->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandLists[0]->GetCommandList()->DrawInstanced(60, 1, 0, 0);


	// This is part of a blit
	auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(GetCurrentBackBuffer(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandLists[0]->GetCommandList()->ResourceBarrier(1, &barrierToPresent);
	
	commandLists[0]->Close();
}

void RenderContext::ExecuteCommandList(size_t cmdListIndex)
{
	ID3D12CommandList* ppCommandLists[] = { commandLists[cmdListIndex]->GetCommandList() };
	deviceContext.GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}

size_t RenderContext::CreateCommandList()
{
	size_t index = commandLists.size();
	commandLists.push_back(new CommandList());
	commandLists[index]->Create();

	return index;
}
