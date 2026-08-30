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
	const float width = 730.0f;
	const float height = 650.0f;
	const float leftMargin = 48.0f;
	const float topMargin = 48.0f;
	const float letterWidth = 39.0f;
	const float letterHeight = 55.0f;

	const HTexture outputTexture = renderContext.GetTexture(renderTarget);
	const D3D12_RESOURCE_DESC outputDesc =
		renderContext.GetTexture(outputTexture)->GetResource()->GetDesc();
	const float renderTargetAspectCorrection =
		static_cast<float>(outputDesc.Height) / static_cast<float>(outputDesc.Width);

	constexpr unsigned int labelSlotCount = 7;
	constexpr unsigned int drawableGlyphCount = 6;
	constexpr unsigned int verticesPerGlyph = 6;
	constexpr unsigned int positionValuesPerVertex = 4;
	constexpr unsigned int colorValuesPerVertex = 4;
	constexpr unsigned int textureCoordinateValuesPerVertex = 2;
	constexpr unsigned int glyphColumns[labelSlotCount] = { 5, 4, 12, 0, 1, 2, 7 };
	constexpr unsigned int glyphRows[labelSlotCount] = { 1, 0, 1, 0, 4, 6, 4 };
	constexpr bool glyphVisible[labelSlotCount] = { true, true, true, false, true, true, true };

	const float quadHeight = 0.0375f;
	const float quadWidth = quadHeight * (letterWidth / letterHeight) * renderTargetAspectCorrection;
	const float horizontalSpacing = 9.0f;  // 48 - 39
	const float verticalSpacing = 25.0f;   // 80 - 55
	const float quadSpacing = quadWidth * (horizontalSpacing / letterWidth);
	const float glyphAdvance = quadWidth + quadSpacing;
	const float labelWidth = labelSlotCount * quadWidth + (labelSlotCount - 1) * quadSpacing;
	const float rightMargin = 0.05f;
	const float bottomMargin = 0.05f;
	const float labelLeft = 1.0f - rightMargin - labelWidth;
	const float quadBottom = -1.0f + bottomMargin;
	const float quadTop = quadBottom + quadHeight;

	float quad[drawableGlyphCount * verticesPerGlyph * positionValuesPerVertex] = {};
	float colors[drawableGlyphCount * verticesPerGlyph * colorValuesPerVertex] = {};
	float textureCoordinates[drawableGlyphCount * verticesPerGlyph * textureCoordinateValuesPerVertex] = {};

	unsigned int drawableGlyphIndex = 0;
	for (unsigned int slotIndex = 0; slotIndex < labelSlotCount; ++slotIndex)
	{
		if (!glyphVisible[slotIndex])
		{
			continue;
		}

		const float quadLeft = labelLeft + slotIndex * glyphAdvance;
		const float quadRight = quadLeft + quadWidth;
		const float glyphLeft =
			leftMargin + glyphColumns[slotIndex] * (letterWidth + horizontalSpacing);
		const float glyphTop =
			topMargin + glyphRows[slotIndex] * (letterHeight + verticalSpacing);
		const float glyphRight = glyphLeft + letterWidth;
		const float glyphBottom = glyphTop + letterHeight;

		const float glyphPositions[] =
		{
			quadLeft,  quadBottom, 0.0f, 1.0f,
			quadRight, quadBottom, 0.0f, 1.0f,
			quadLeft,  quadTop,    0.0f, 1.0f,

			quadRight, quadBottom, 0.0f, 1.0f,
			quadRight, quadTop,    0.0f, 1.0f,
			quadLeft,  quadTop,    0.0f, 1.0f
		};
		const float glyphTextureCoordinates[] =
		{
			glyphLeft / width, glyphBottom / height,
			glyphRight / width, glyphBottom / height,
			glyphLeft / width, glyphTop / height,

			glyphRight / width, glyphBottom / height,
			glyphRight / width, glyphTop / height,
			glyphLeft / width, glyphTop / height
		};

		const unsigned int positionOffset =
			drawableGlyphIndex * verticesPerGlyph * positionValuesPerVertex;
		for (unsigned int valueIndex = 0;
			valueIndex < verticesPerGlyph * positionValuesPerVertex;
			++valueIndex)
		{
			quad[positionOffset + valueIndex] = glyphPositions[valueIndex];
			colors[positionOffset + valueIndex] = 1.0f;
		}

		const unsigned int textureCoordinateOffset =
			drawableGlyphIndex * verticesPerGlyph * textureCoordinateValuesPerVertex;
		for (unsigned int valueIndex = 0;
			valueIndex < verticesPerGlyph * textureCoordinateValuesPerVertex;
			++valueIndex)
		{
			textureCoordinates[textureCoordinateOffset + valueIndex] =
				glyphTextureCoordinates[valueIndex];
		}

		++drawableGlyphIndex;
	}

	VertexBufferCreateDesc positionDesc;
	positionDesc.numOfVertices = drawableGlyphCount * verticesPerGlyph;
	positionDesc.numOfFloatsPerVertex = 4;
	positionDesc.name = "UI_Label_Position";
	HVertexBuffer position = renderContext.CreateVertexBuffer(positionDesc, quad);

	VertexBufferCreateDesc colorDesc;
	colorDesc.numOfVertices = drawableGlyphCount * verticesPerGlyph;
	colorDesc.numOfFloatsPerVertex = 4;
	colorDesc.name = "UI_Label_Color";
	HVertexBuffer color = renderContext.CreateVertexBuffer(colorDesc, colors);

	VertexBufferCreateDesc textureCoordinateDesc;
	textureCoordinateDesc.numOfVertices = drawableGlyphCount * verticesPerGlyph;
	textureCoordinateDesc.numOfFloatsPerVertex = 2;
	textureCoordinateDesc.name = "UI_Label_TextureCoordinate";
	HVertexBuffer textureCoordinate = renderContext.CreateVertexBuffer(textureCoordinateDesc, textureCoordinates);

	letterMesh = renderContext.CreateMesh(position, color, textureCoordinate,
		HVertexBuffer::Invalid(), "UI_Label_Mesh");
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
