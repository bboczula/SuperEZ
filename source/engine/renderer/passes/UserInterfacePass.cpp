#include "UserInterfacePass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../core/InputLayout.h"
#include "../../engine/EngineAssets.h"

extern RenderContext renderContext;

UserInterfacePass::UserInterfacePass(const EngineAssets& engineAssets)
	: RenderPass(L"UserInterface", L"user_interface.hlsl", Type::Graphics),
	engineAssets(engineAssets)
{
}

UserInterfacePass::~UserInterfacePass()
{
}

void UserInterfacePass::ConfigurePipelineState()
{
    // Pre-AutomaticInitialize Procedure
    inputLayout = renderContext.CreateInputLayout();
    renderContext.GetInputLayout(inputLayout)->AppendElementT(
		VertexStream::Position,
		VertexStream::Color,
		VertexStream::TexCoord);

    // Now we can create the root signature
    RootSignatureBuilder builder;
	builder.AddSRVTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
	builder.AddSamplerTable(0, 1, D3D12_SHADER_VISIBILITY_PIXEL);
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
	float colors[] =
	{
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,

		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f
	};
	float textureCoordinates[] =
	{
		0.0f, 1.0f,
		1.0f, 1.0f,
		0.0f, 0.0f,

		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f
	};

	VertexBufferCreateDesc positionDesc;
	positionDesc.numOfVertices = 6;
	positionDesc.numOfFloatsPerVertex = 4;
	positionDesc.name = "UI_Letter_Position";
	HVertexBuffer position = renderContext.CreateVertexBuffer(positionDesc, quad);

	VertexBufferCreateDesc colorDesc;
	colorDesc.numOfVertices = 6;
	colorDesc.numOfFloatsPerVertex = 4;
	colorDesc.name = "UI_Letter_Color";
	HVertexBuffer color = renderContext.CreateVertexBuffer(colorDesc, colors);

	VertexBufferCreateDesc textureCoordinateDesc;
	textureCoordinateDesc.numOfVertices = 6;
	textureCoordinateDesc.numOfFloatsPerVertex = 2;
	textureCoordinateDesc.name = "UI_Letter_TextureCoordinate";
	HVertexBuffer textureCoordinate = renderContext.CreateVertexBuffer(textureCoordinateDesc, textureCoordinates);

	letterMesh = renderContext.CreateMesh(position, color, textureCoordinate,
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
	renderContext.SetDescriptorHeap(commandList);

	HTexture outputTexture = renderContext.GetTexture(renderTarget);
	renderContext.TransitionTo(commandList, outputTexture, D3D12_RESOURCE_STATE_RENDER_TARGET);
	renderContext.BindRenderTarget(commandList, renderTarget);

	renderContext.BindTexture(commandList, engineAssets.bitmapFont, 0);
	renderContext.BindGeometry(commandList, letterMesh);
	renderContext.DrawMesh(commandList, letterMesh);
}

void UserInterfacePass::PostSubmit()
{
}

void UserInterfacePass::Allocate(DeviceContext* deviceContext)
{
}
