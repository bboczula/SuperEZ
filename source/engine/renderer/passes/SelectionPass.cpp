#include "SelectionPass.h"
#include "../RenderContext.h"
#include "../../core/InputLayout.h"
#include "../../core/DeviceContext.h"
#include "../../engine/camera/Camera.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../engine/input/RawInput.h"
#include "../../engine/input/CursorInput.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;
extern RawInput rawInput;
extern CursorInput cursorInput;

SelectionPass::SelectionPass() : RenderPass(L"Selection", L"selection.hlsl", Type::Graphics)
{
	deviceContext.GetDevice()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&readbackFence));
	readbackFence->SetName(L"Selection Pass Fence");

	readbackEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr); // auto-reset
	if (!readbackEvent)
		OutputDebugString(L"ERROR: CreateEvent failed for SelectionPass readbackEvent\n");

	fenceValue = 0;
	lastRequestedFenceValue = 0;
	waitingForReadback = false;
}

SelectionPass::~SelectionPass()
{
	if (readbackEvent)
	{
		CloseHandle(readbackEvent);
		readbackEvent = nullptr;
	}
}

void SelectionPass::ConfigurePipelineState()
{
	// Pre-AutomaticInitialize Procedure
	inputLayout = renderContext.CreateInputLayout();
	renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);

	// Now we can create the root signature
	RootSignatureBuilder builder;
	builder.AddConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddConstants(16, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddConstants(1, 2, 0, D3D12_SHADER_VISIBILITY_PIXEL); // Root Constants @ b1
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // SRV t0
	builder.AddSamplerTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL); // Sampler s0
	rootSignature = renderContext.CreateRootSignature(builder);

	// Menu height seems to be 20 pixels
	// The actual viewport size is this ImVec2(1920 - 400, 1080 - menuHeight - 25);
	int viewportWidth = 1920;
	int viewportHeight = 1080;
#if IS_EDITOR
	const int menuHeight = 20;
	viewportWidth -= 400; // Assuming the menu takes 400 pixels
	viewportHeight -= menuHeight - 25; // Assuming the status bar takes 25 pixels
#endif
	renderTarget = renderContext.CreateRenderTarget("RT_Selection", RenderTargetFormat::R32_UINT, viewportWidth, viewportHeight);
	depthBuffer = renderContext.CreateDepthBuffer();
	readbackBuffer = renderContext.CreateReadbackBuffer();
	deviceContext.Flush();
}

void SelectionPass::PostAssetLoad()
{
}

void SelectionPass::Initialize()
{
}

void SelectionPass::Update()
{
}

void SelectionPass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeap(commandList);
	renderContext.BindRenderTargetWithDepth(commandList, renderTarget, depthBuffer);
	renderContext.CleraRenderTarget(commandList, renderTarget);
	renderContext.ClearDepthBuffer(commandList, depthBuffer);

	renderContext.SetInlineConstants(commandList, renderContext.GetCamera(0)->ViewProjecttion(), 0);

	UINT index = 0;
	const auto& items = renderContext.GetRenderItems();
	for (const RenderItem& item : items)
	{
		UINT id = index + 1;
		renderContext.SetInlineConstants(commandList, id, 2);
		renderContext.SetInlineConstants(commandList, item.World(), 1);
		renderContext.BindGeometry(commandList, item.mesh);
		renderContext.BindTexture(commandList, item.texture, 3);
		renderContext.DrawMesh(commandList, item.mesh);
		index++;
	}

	// Here we can pass the mouse position to the shader
	auto mousePositionX = cursorInput.GetMouseX();
	auto mousePositionY = cursorInput.GetMouseY();
#if IS_EDITOR
	mousePositionX -= 400;
	mousePositionY -= 20; // Adjust for the menu height
#endif

	// Only request a readback if the user actually clicked this frame
	// Or if something has changed that requires a new readback, like selected object moved
	if (cursorInput.WasLeftButtonClicked() && mousePositionX >= 0 && mousePositionY >= 0)
	{
		auto texture = renderContext.GetTexture(renderTarget);
		renderContext.TransitionTo(commandList, texture, D3D12_RESOURCE_STATE_COPY_SOURCE);
		renderContext.CopyTextureToBuffer(commandList, texture, readbackBuffer, mousePositionX, mousePositionY);
		renderContext.TransitionBack(commandList, texture);

		pendingFenceValue = ++fenceValue;
		pendingReadbackSignal = true;

		renderContext.SetWasObjectSelected(true);
	}
	else if (cursorInput.WasLeftButtonClicked())
	{
		renderContext.SetSelectedObjectId(UINT32_MAX);
		renderContext.SetWasObjectSelected(false);
	}

}

void SelectionPass::PostSubmit()
{
	if (!pendingReadbackSignal)
		return;

	// 1) Signal fence AFTER the command list containing the copy is submitted
	deviceContext.GetCommandQueue()->Signal(readbackFence, pendingFenceValue);

	lastRequestedFenceValue = pendingFenceValue;
	pendingReadbackSignal = false;

	// 2) FIX #2: hard stall until GPU finishes the copy
	if (readbackFence->GetCompletedValue() < lastRequestedFenceValue)
	{
		if (readbackEvent)
		{
			readbackFence->SetEventOnCompletion(lastRequestedFenceValue, readbackEvent);
			WaitForSingleObject(readbackEvent, INFINITE);
		}
		else
		{
			// Fallback (worse): spin
			while (readbackFence->GetCompletedValue() < lastRequestedFenceValue) {}
		}
	}

	// 3) Now readback buffer is guaranteed ready THIS FRAME
	ApplyReadbackSelection();

	// 4) We handled it immediately; don't also poll in Update()
	waitingForReadback = false;
}


void SelectionPass::ApplyReadbackSelection()
{
	auto data = renderContext.ReadbackBufferData(readbackBuffer, sizeof(UINT32));
	if (data.size() < sizeof(UINT32))
		return;

	const UINT32 rawID = *reinterpret_cast<const UINT32*>(data.data());

	if (rawID != 0) // 0 = background
		renderContext.SetSelectedObjectId(rawID - 1);
	else
		renderContext.SetSelectedObjectId(UINT32_MAX);
}


void SelectionPass::Allocate(DeviceContext* deviceContext)
{
}
