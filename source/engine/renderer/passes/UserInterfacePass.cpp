#include "UserInterfacePass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../core/InputLayout.h"

extern RenderContext renderContext;

UserInterfacePass::UserInterfacePass() : RenderPass(L"UserInterface", L"user_interface.hlsl", Type::Graphics)
{
}

UserInterfacePass::~UserInterfacePass()
{
}

void UserInterfacePass::ConfigurePipelineState()
{
    // Pre-AutomaticInitialize Procedure
    inputLayout = renderContext.CreateInputLayout();
    renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);

    // Now we can create the root signature
    RootSignatureBuilder builder;
    rootSignature = renderContext.CreateRootSignature(builder);

	HTexture compositionTexture = renderContext.GetTexture("CompositionTexture");
	renderTarget = renderContext.CreateRenderTarget("RT_UserInterfacePass", compositionTexture);
}

void UserInterfacePass::PostAssetLoad()
{
	float quad[] =
	{
		0.78f, -0.15f, 0.0f, 1.0f,
		0.98f, -0.15f, 0.0f, 1.0f,
		0.78f,  0.15f, 0.0f, 1.0f,

		0.98f, -0.15f, 0.0f, 1.0f,
		0.98f,  0.15f, 0.0f, 1.0f,
		0.78f,  0.15f, 0.0f, 1.0f
	};

	VertexBufferCreateDesc desc;
	desc.numOfVertices = 6;
	desc.numOfFloatsPerVertex = 4;
	desc.name = "UI_Letter";

	HVertexBuffer letter = renderContext.CreateVertexBuffer(desc, quad);

	letterMesh = renderContext.CreateMesh(letter, HVertexBuffer::Invalid(), HVertexBuffer::Invalid(),
		HVertexBuffer::Invalid(), "UI_Letter_Mesh");
}

void UserInterfacePass::Initialize()
{
}

void UserInterfacePass::Update()
{
}

void UserInterfacePass::Execute()
{
	renderContext.SetupRenderPass(commandList, pipelineState, rootSignature);

	HTexture outputTexture = renderContext.GetTexture(renderTarget);
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_RENDER_TARGET);
	renderContext.BindRenderTarget(commandList, renderTarget);

	renderContext.BindGeometry(commandList, letterMesh);
	renderContext.DrawMesh(commandList, letterMesh);
}

void UserInterfacePass::PostSubmit()
{
}

void UserInterfacePass::Allocate(DeviceContext* deviceContext)
{
}
