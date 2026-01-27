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
    builder.AddConstants(16, 1, 0, D3D12_SHADER_VISIBILITY_ALL); // Root Constants @ b1
    rootSignature = renderContext.CreateRootSignature(builder);

    int viewportWidth = 1920;
    int viewportHeight = 1080;
#if IS_EDITOR
    const int menuHeight = 20;
    viewportWidth -= 400; // Assuming the menu takes 400 pixels
    viewportHeight -= menuHeight - 25; // Assuming the status bar takes 25 pixels
#endif
    //renderTarget = renderContext.CreateRenderTarget("RT_HighlightInputPass", RenderTargetFormat::R32G32_UINT, viewportWidth, viewportHeight);
    renderTarget = renderContext.CreateRenderTarget("RT_HighlightInputPass", HTexture(6));
    depthBuffer = renderContext.CreateDepthBuffer();
}

void HighlightInputPass::PostAssetLoad()
{
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
	renderContext.SetInlineConstants(commandList, renderContext.GetCamera(0)->ViewProjecttion(), 0);

	static uint32_t selectedIndex = UINT32_MAX;
	auto selectedObjectId = renderContext.GetSelectedObjectId();
	if (selectedObjectId >= renderContext.GetNumOfMeshes())
	{
		return;
	}
	//{
	//	renderContext.BindGeometry(commandList, HMesh(selectedObjectId));
	//	renderContext.DrawMesh(commandList, HMesh(selectedObjectId));
	//}

	RenderItem item = renderContext.GetRenderItems()[selectedObjectId];
	renderContext.SetInlineConstants(commandList, item.World(), 1);
	renderContext.BindGeometry(commandList, item.mesh);
	renderContext.DrawMesh(commandList, item.mesh);

	//const auto& items = renderContext.GetRenderItems();
	//for (const RenderItem& item : items)
	//{
	//	renderContext.SetInlineConstants(commandList, item.World(), 1);
	//	renderContext.BindGeometry(commandList, item.mesh);
	//	renderContext.BindTexture(commandList, item.texture, 2);
	//	renderContext.DrawMesh(commandList, item.mesh);
	//}

	renderContext.TransitionBack(commandList, renderContext.GetTexture(renderTarget));
}

void HighlightInputPass::PostSubmit()
{
}

void HighlightInputPass::Allocate(DeviceContext* deviceContext)
{
}
