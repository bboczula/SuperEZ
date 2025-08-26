#include "SelectionPass.h"
#include "../RenderContext.h"
#include "../../core/InputLayout.h"
#include "../../core/DeviceContext.h"
#include "../../engine/camera/Camera.h"
#include "../RootSignatureBuilder.h"
#include "../../engine/input/RawInput.h"
#include "../../engine/input/CursorInput.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;
extern RawInput rawInput;
extern CursorInput cursorInput;

SelectionPass::SelectionPass() : RenderPass(L"Selection", L"selection.hlsl", Type::Default)
{
	deviceContext.GetDevice()->CreateFence(
		0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&readbackFence)
	);
	readbackFence->SetName(L"Selection Pass Fence");

	readbackEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
	if (!readbackEvent)
		exit(1);

	fenceValue = 1; // start at 1 for first signal
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
	const int menuHeight = 20;
	const int viewportWidth = 1920 - 400; // Assuming the menu takes 400 pixels
	const int viewportHeight = 1080 - menuHeight - 25; // Assuming the status bar takes 25 pixels
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
	if (skipFrame)
	{
		skipFrame = false;
		return; // Skip the frame if no selection is made
	}

	if (readbackFence->GetCompletedValue() >= fenceValue)
	{
		// Reset the fence value for the next frame
		fenceValue++;

		if (!renderContext.WasObjectSelected())
		{
			return; // Skip if the viewport was not clicked
		}

		auto data = renderContext.ReadbackBufferData(readbackBuffer, sizeof(UINT32));

		if (!data.empty())
		{
			UINT32 objectID = *reinterpret_cast<const UINT32*>(data.data());
			if(objectID != 0)
			{
				renderContext.SetSelectedObjectId(objectID - 1); // Adjust for 0-based index
			}
			else
			{
				renderContext.SetSelectedObjectId(UINT32_MAX); // Reset selection if no object is selected
			}
		}
	}
	else
	{
		HANDLE eventHandle = readbackEvent;
		if (eventHandle)
		{
			WaitForSingleObject(eventHandle, INFINITE);
		}
	}

}

void SelectionPass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature, viewportAndScissors);
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
	mousePositionX -= 400;
	mousePositionY -= 20; // Adjust for the menu height

	if (cursorInput.WasLeftButtonClicked())
	{
		const bool wasViewportClicked = mousePositionX > 0 && mousePositionY > 0;
		if (wasViewportClicked)
		{
			auto texture = renderContext.GetTexture(renderTarget);
			renderContext.CopyTextureToBuffer(commandList, texture, readbackBuffer, mousePositionX, mousePositionY);
		}
		else
		{
			// Reset selection if clicked outside the viewport
			renderContext.SetSelectedObjectId(UINT32_MAX);
		}
		renderContext.SetWasObjectSelected(wasViewportClicked);
	}

	// Signal
	deviceContext.GetCommandQueue()->Signal(readbackFence, fenceValue);
	skipFrame = false;
}

void SelectionPass::Allocate(DeviceContext* deviceContext)
{
}
