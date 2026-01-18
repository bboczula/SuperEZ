#include "HighlightInputPass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../core/InputLayout.h"
#include "../../engine/camera/Camera.h"

extern RenderContext renderContext;

HighlightInputPass::HighlightInputPass() : RenderPass(L"HighlightInput", L"highlight_input.hlsl", Type::Graphics)
{
}

HighlightInputPass::~HighlightInputPass()
{
}

void HighlightInputPass::ConfigurePipelineState()
{
    // Pre-AutomaticInitialize Procedure
    inputLayout = renderContext.CreateInputLayout();
    renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);

    // Now we can create the root signature
    RootSignatureBuilder builder;
    builder.AddConstants(16, 0, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b0
    rootSignature = renderContext.CreateRootSignature(builder);

    const int menuHeight = 20;
    const int viewportWidth = 1920 - 400; // Assuming the menu takes 400 pixels
    const int viewportHeight = 1080 - menuHeight - 25; // Assuming the status bar takes 25 pixels
    //renderTarget = renderContext.CreateRenderTarget("RT_HighlightInputPass", RenderTargetFormat::R32G32_UINT, viewportWidth, viewportHeight);
    renderTarget = renderContext.CreateRenderTarget("RT_HighlightInputPass", HTexture(6));
    depthBuffer = renderContext.CreateDepthBuffer();
}

void HighlightInputPass::Initialize()
{
}

void HighlightInputPass::Update()
{
}

void HighlightInputPass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);
	renderContext.SetDescriptorHeap(commandList);
	renderContext.TransitionTo(commandList, renderContext.GetTexture(renderTarget), D3D12_RESOURCE_STATE_RENDER_TARGET);
	renderContext.BindRenderTargetWithDepth(commandList, renderTarget, depthBuffer);
	renderContext.ClearDepthBuffer(commandList, depthBuffer);
	renderContext.SetInlineConstants(commandList, 16, renderContext.GetCamera(0)->GetViewProjectionMatrixPtr(), 0);

	static uint32_t selectedIndex = UINT32_MAX;
	auto selectedObjectId = renderContext.GetSelectedObjectId();
	if (selectedObjectId < renderContext.GetNumOfMeshes())
	{
		renderContext.BindGeometry(commandList, HMesh(selectedObjectId));
		renderContext.DrawMesh(commandList, HMesh(selectedObjectId));
	}
	renderContext.TransitionBack(commandList, renderContext.GetTexture(renderTarget));
}

void HighlightInputPass::Allocate(DeviceContext* deviceContext)
{
}
