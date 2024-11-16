#include "RenderContext.h"
#include "DeviceContext.h"
#include "WindowContext.h"

#include <Windows.h>
#include <d3dcompiler.h>
#include "../externals/SimpleMath/SimpleMath.h"
#include "debugapi.h"
#include "Utils.h"

extern WindowContext windowContext;

RenderContext::RenderContext()
{
	OutputDebugString(L"RenderContext Constructor\n");
}

RenderContext::~RenderContext()
{
	OutputDebugString(L"RenderContext Destructor\n");

	// Reelease Render Resources
	SafeRelease(&pipelineState);
	SafeRelease(&vertexShader);
	SafeRelease(&rootSignature);

	SafeRelease(&commandList);
	SafeRelease(&commandAllocator);
	for (int i = 0; i < FRAME_COUNT; i++)
	{
		SafeRelease(&backBuffer[i]);
	}
	SafeRelease(&rtvHeap);
	SafeRelease(&dsvHeap);
	SafeRelease(&cbvSrvUavHeap);
}

void RenderContext::CreateDescriptorHeap(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateDescriptorHeap\n");

	D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
	rtvHeapDesc.NumDescriptors = FRAME_COUNT;
	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	ExitIfFailed(deviceContext->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&rtvHeap)));
	rtvHeap->SetName(L"RTV Heap");
	OutputDebugString(L"Successfully created RTV descriptor heap\n");

	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	ExitIfFailed(deviceContext->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&dsvHeap)));
	dsvHeap->SetName(L"DSV Heap");
	OutputDebugString(L"Successfully created DSV descriptor heap\n");

	rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	rtvHeapDesc.Flags |= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	ExitIfFailed(deviceContext->GetDevice()->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&cbvSrvUavHeap)));
	cbvSrvUavHeap->SetName(L"CBV_SRV_UAV Heap");
	OutputDebugString(L"Successfully created CBV_SRV_UAV descriptor heap\n");

	rtvHeapDescriptorSize = deviceContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	dsvHeapDescriptorSize = deviceContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	cbvSrvUavHeapDescriptorSize = deviceContext->GetDevice()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
}

void RenderContext::CreateCommandBuffer(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateCommandBuffer\n");

	ExitIfFailed(deviceContext->GetDevice()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&commandAllocator)));
	commandAllocator->SetName(L"Render Context Command Allocator");
	ExitIfFailed(deviceContext->GetDevice()->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, commandAllocator, nullptr, IID_PPV_ARGS(&commandList)));
	commandList->SetName(L"Render Context Command List");
	commandList->Close();

	OutputDebugString(L"CreateCommandBuffer succeeded\n");
}

void RenderContext::CreateRenderTarget(DeviceContext* deviceContext)
{
}

void RenderContext::CreateRenderTargetFromBackBuffer(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateRenderTargetFromBackBuffer\n");
	// We need to get the descriptor handle for the first RTV
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart());
	
	// Create a RTV for each frame
	for (UINT i = 0; i < FRAME_COUNT; i++)
	{
		// Here, we don't really create the underlying resource for the RTV
		// We just get the back buffer from the swap chain and create a RTV for it
		IDXGISwapChain* swapChain = deviceContext->GetSwapChain();
		ExitIfFailed(swapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer[i])));
		backBuffer[i]->SetName(L"Render Context Back Buffer");
		deviceContext->GetDevice()->CreateRenderTargetView(backBuffer[i], nullptr, rtvHandle);
		rtvHandle.Offset(1, rtvHeapDescriptorSize);
		OutputDebugString(L"CreateRenderTargetFromBackBuffer succeeded\n");
	}
}

void RenderContext::CreateRootSignature(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateRootSignature\n");

	CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob* signature;
	ID3DBlob* error;
	ExitIfFailed(D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error));
	ExitIfFailed(deviceContext->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSignature)));
	rootSignature->SetName(L"Render Context Root Signature");
	OutputDebugString(L"CreateRootSignature succeeded\n");
}

void RenderContext::CreateShaders(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateShaders\n");

#if defined(DEBUG)
	// Enable better shader debugging with the graphics debugging tools.
	UINT compileFlags = D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION;
#else
	UINT compileFlags = 0;
#endif

#if defined(DEBUG)
	// If you run from VS, the working directory is build, so we need to go up one level
	ExitIfFailed(D3DCompileFromFile(L"../source/engine/shaders/shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
	ExitIfFailed(D3DCompileFromFile(L"../source/engine/shaders/shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
#else
	// In final we will copy shaders to the bin directory
	ExitIfFailed(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "VSMain", "vs_5_0", compileFlags, 0, &vertexShader, nullptr));
	ExitIfFailed(D3DCompileFromFile(L"shaders.hlsl", nullptr, nullptr, "PSMain", "ps_5_0", compileFlags, 0, &pixelShader, nullptr));
#endif
	OutputDebugString(L"CreateShaders succeeded\n");
}

void RenderContext::CreatePipelineState(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreatePipelineState\n");

	// Define the vertex input layout.
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[] =
	{
	    { "POSITION", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	    //{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// Describe and create the graphics pipeline state object (PSO).
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = { inputElementDescs, _countof(inputElementDescs) };
	psoDesc.pRootSignature = rootSignature;
	psoDesc.VS = CD3DX12_SHADER_BYTECODE(vertexShader);
	psoDesc.PS = CD3DX12_SHADER_BYTECODE(pixelShader);
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_BACK;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState.DepthEnable = FALSE;
	psoDesc.DepthStencilState.StencilEnable = FALSE;
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	psoDesc.SampleDesc.Count = 1;
	ExitIfFailed(deviceContext->GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState)));
	pipelineState->SetName(L"Render Context Pipeline State");
	OutputDebugString(L"CreatePipelineState succeeded\n");
}

void RenderContext::CreateViewportAndScissorRect(DeviceContext* deviceContext)
{
	OutputDebugString(L"CreateViewportAndScissorRect\n");

	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(windowContext.GetWidth()), static_cast<float>(windowContext.GetHeight()));
	scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(windowContext.GetWidth()), static_cast<LONG>(windowContext.GetHeight()));
	OutputDebugString(L"CreateViewportAndScissorRect succeeded\n");
}

void RenderContext::CreateVertexBuffer(DeviceContext* deviceContext)
{
	float ratio = static_cast<float>(windowContext.GetWidth()) / static_cast<float>(windowContext.GetHeight());
	DirectX::SimpleMath::Vector4 triangleVertices[] =
	{
		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.079f, 0.097f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.0f, 0.0f * ratio, 0.0f, 1.0f),

		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.0f, 0.0f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(-0.079f, 0.097f * ratio, 0.0f, 1.0f),

		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.168f, 0.21f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.079f, 0.097f * ratio, 0.0f, 1.0f),

		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(-0.079f, 0.097f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(-0.168f, 0.21f * ratio, 0.0f, 1.0f),

		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.079f, 0.384f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.168f, 0.21f * ratio, 0.0f, 1.0f),

		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(-0.168f, 0.21f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(-0.079f, 0.384f * ratio, 0.0f, 1.0f),

		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.0f, 0.5f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.079f, 0.384f * ratio, 0.0f, 1.0f),

		DirectX::SimpleMath::Vector4(0.0f, 0.289f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(-0.079f, 0.384f * ratio, 0.0f, 1.0f),
		DirectX::SimpleMath::Vector4(0.0f, 0.5f * ratio, 0.0f, 1.0f)
	};

	const UINT vertexBufferSize = sizeof(triangleVertices);

	UINT bufferId = deviceContext->CreateVertexBuffer(vertexBufferSize);
	
	// Copy the triangle data to the vertex buffer.
	UINT8* pVertexDataBegin;
	CD3DX12_RANGE readRange(0, 0);        // We do not intend to read from this resource on the CPU.
	
	auto vb = deviceContext->GetVertexBuffer(bufferId);
	ExitIfFailed(vb->Map(0, &readRange, reinterpret_cast<void**>(&pVertexDataBegin)));
	memcpy(pVertexDataBegin, triangleVertices, sizeof(triangleVertices));
	vb->Unmap(0, nullptr);
	
	// Initialize the vertex buffer view.
	vertexBufferView.BufferLocation = vb->GetGPUVirtualAddress();
	vertexBufferView.StrideInBytes = sizeof(DirectX::SimpleMath::Vector4);
	vertexBufferView.SizeInBytes = vertexBufferSize;
}

void RenderContext::PopulateCommandList(DeviceContext* deviceContext)
{
	auto frameIndex = deviceContext->GetCurrentBackBufferIndex();
	ExitIfFailed(commandAllocator->Reset());
	ExitIfFailed(commandList->Reset(commandAllocator, pipelineState));
	commandList->SetGraphicsRootSignature(rootSignature);
	commandList->RSSetViewports(1, &viewport);
	commandList->RSSetScissorRects(1, &scissorRect);
	auto barrierToRenderTarget = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer[frameIndex], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	commandList->ResourceBarrier(1, &barrierToRenderTarget);
	//commandList->ResourceBarrier(1, &CD3DX12_RESOURCE_BARRIER::Transition(backBuffer[deviceContext->GetCurrentBackBufferIndex()], D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET));
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(rtvHeap->GetCPUDescriptorHandleForHeapStart(), deviceContext->GetCurrentBackBufferIndex(), rtvHeapDescriptorSize);
	commandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	float clearColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
	commandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);

	commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	commandList->IASetVertexBuffers(0, 1, &vertexBufferView);
	commandList->DrawInstanced(24, 1, 0, 0);

	auto barrierToPresent = CD3DX12_RESOURCE_BARRIER::Transition(backBuffer[frameIndex], D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	commandList->ResourceBarrier(1, &barrierToPresent);
	ExitIfFailed(commandList->Close());
}

void RenderContext::ExecuteCommandList(DeviceContext* deviceContext)
{
	ID3D12CommandList* ppCommandLists[] = { commandList };
	deviceContext->GetCommandQueue()->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}
