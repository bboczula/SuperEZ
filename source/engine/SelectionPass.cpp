#include "SelectionPass.h"
#include "RenderContext.h"
#include "InputLayout.h"
#include "DeviceContext.h"
#include "camera/Camera.h"
#include "RootSignatureBuilder.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

SelectionPass::SelectionPass() : RenderPass(L"Selection", L"selection.hlsl", Type::Default)
{
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
}

void SelectionPass::Initialize()
{
	renderTarget = renderContext.CreateRenderTarget("RT_Selection", RenderTargetFormat::R32_UINT);
	depthBuffer = renderContext.CreateDepthBuffer();
	renderContext.CreateReadbackBuffer();
	deviceContext.Flush();
}

void SelectionPass::Update()
{
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
		UINT id = i;
		renderContext.SetInlineConstants(commandList, 1, &id, 1);
		renderContext.BindGeometry(commandList, HMesh(i));
		renderContext.BindTexture(commandList, HTexture(i), 2);
		renderContext.DrawMesh(commandList, HMesh(i));
	}
}

void SelectionPass::Allocate(DeviceContext* deviceContext)
{
}
