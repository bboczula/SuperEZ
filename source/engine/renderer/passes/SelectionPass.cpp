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

	// We don't strictly need an Event for polling, but if you keep it, ensure you don't use it to block the main thread.
	fenceValue = 0;
	lastRequestedFenceValue = 0;
	waitingForReadback = false;
}

void SelectionPass::ConfigurePipelineState()
{
	// Pre-AutomaticInitialize Procedure
	inputLayout = renderContext.CreateInputLayout();
	renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);

	// Now we can create the root signature
	RootSignatureBuilder builder;
	builder.AddConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
	builder.AddConstants(1, 1, 0, D3D12_SHADER_VISIBILITY_PIXEL); // Root Constants @ b1
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

void SelectionPass::Initialize()
{
}

void SelectionPass::Update()
{
	// 1. Check if we are waiting for data from the GPU
	if (waitingForReadback)
	{
		// NON-BLOCKING CHECK: Is the GPU done with the frame where we clicked?
		if (readbackFence->GetCompletedValue() >= lastRequestedFenceValue)
		{
			// GPU is done! Read the data.
			auto data = renderContext.ReadbackBufferData(readbackBuffer, sizeof(UINT32));
			if (!data.empty())
			{
				UINT32 rawID = *reinterpret_cast<const UINT32*>(data.data());

				if (rawID != 0) // Assuming 0 is background
					renderContext.SetSelectedObjectId(rawID - 1);
				else
					renderContext.SetSelectedObjectId(UINT32_MAX);
			}

			// Reset state
			waitingForReadback = false;
		}
		// If fence is not ready, we simply RETURN and try again next frame. 
		// DO NOT WaitForSingleObject here.
	}

}

void SelectionPass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeap(commandList);
	renderContext.BindRenderTargetWithDepth(commandList, renderTarget, depthBuffer);
	renderContext.CleraRenderTarget(commandList, renderTarget);
	renderContext.ClearDepthBuffer(commandList, depthBuffer);

	renderContext.SetInlineConstants(commandList, 16, renderContext.GetCamera(0)->GetViewProjectionMatrixPtr(), 0);

	for (int i = 0; i < renderContext.GetNumOfMeshes(); i++)
	{
		UINT id = i + 1;
		renderContext.SetInlineConstants(commandList, 1, &id, 1);
		renderContext.BindGeometry(commandList, HMesh(i));
		renderContext.BindTexture(commandList, HTexture(i), 2);
		renderContext.DrawMesh(commandList, HMesh(i));
	}

	// Here we can pass the mouse position to the shader
	auto mousePositionX = cursorInput.GetMouseX();
	auto mousePositionY = cursorInput.GetMouseY();
#if IS_EDITOR
	mousePositionX -= 400;
	mousePositionY -= 20; // Adjust for the menu height
#endif

	// Only request a readback if the user actually clicked this frame
	if (cursorInput.WasLeftButtonClicked() && mousePositionX > 0 && mousePositionY > 0)
	{
		// 1. Copy the specific pixel to the readback buffer
		auto texture = renderContext.GetTexture(renderTarget);
		renderContext.TransitionTo(commandList, texture, D3D12_RESOURCE_STATE_COPY_SOURCE);
		renderContext.CopyTextureToBuffer(commandList, texture, readbackBuffer, mousePositionX, mousePositionY);
		renderContext.TransitionBack(commandList, texture);

		// 2. Increment fence value for this new request
		fenceValue++;

		// 3. Insert Signal into Command List
		deviceContext.GetCommandQueue()->Signal(readbackFence, fenceValue);

		// 4. Update Internal State
		lastRequestedFenceValue = fenceValue;
		waitingForReadback = true;

		// Update the context immediately so UI feels responsive (optimistic update),
		// or wait for the readback (conservative).
		renderContext.SetWasObjectSelected(true);
	}
	else if (cursorInput.WasLeftButtonClicked())
	{
		// Clicked outside viewport -> Deselect immediately
		renderContext.SetSelectedObjectId(UINT32_MAX);
		renderContext.SetWasObjectSelected(false);
	}
}

void SelectionPass::Allocate(DeviceContext* deviceContext)
{
}
