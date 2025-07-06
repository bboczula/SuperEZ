#include "SelectionPass.h"
#include "RenderContext.h"
#include "InputLayout.h"
#include "DeviceContext.h"

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
}

void SelectionPass::Initialize()
{
	renderTarget = renderContext.CreateRenderTarget();
	depthBuffer = renderContext.CreateDepthBuffer();
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

	//auto type = isPerspectiveCamera ? Camera::CameraType::PERSPECTIVE : Camera::CameraType::ORTHOGRAPHIC;
	//renderContext.SetInlineConstants(commandList, 16, camera->GetViewProjectionMatrixPtr(type));

	for (int i = 0; i < renderContext.GetNumOfMeshes(); i++)
	{
		renderContext.BindGeometry(commandList, HMesh(i));
		renderContext.BindTexture(commandList, HTexture(i));
		renderContext.DrawMesh(commandList, HMesh(i));
	}
}

void SelectionPass::Allocate(DeviceContext* deviceContext)
{
}
