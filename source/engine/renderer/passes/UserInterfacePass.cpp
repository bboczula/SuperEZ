#include "UserInterfacePass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../core/InputLayout.h"
#include "../../engine/EngineAssets.h"

#include <vector>

extern RenderContext renderContext;

namespace
{
	struct FontAtlasData
	{
		float width;
		float height;
		float leftMargin;
		float topMargin;
		float glyphWidth;
		float glyphHeight;
		float horizontalSpacing;
		float verticalSpacing;
	};

	struct GlyphData
	{
		unsigned int column;
		unsigned int row;
		bool isVisible;
	};

	struct LabelData
	{
		const GlyphData* glyphs;
		unsigned int glyphCount;
		float height;
		float rightMargin;
		float bottomMargin;
	};

	struct LabelLayout
	{
		float width;
		float left;
		float bottom;
		float top;
		float glyphWidth;
		float glyphHeight;
		float glyphSpacing;
		float glyphAdvance;
	};

	struct Glyph
	{
		float left;
		float right;
		float bottom;
		float top;
		float textureLeft;
		float textureRight;
		float textureBottom;
		float textureTop;
	};

	struct LabelMeshData
	{
		std::vector<float> positions;
		std::vector<float> colors;
		std::vector<float> textureCoordinates;
	};

	constexpr unsigned int verticesPerGlyph = 6;
	constexpr unsigned int positionValuesPerVertex = 4;
	constexpr unsigned int colorValuesPerVertex = 4;
	constexpr unsigned int textureCoordinateValuesPerVertex = 2;

	constexpr GlyphData versionLabelGlyphs[] =
	{
		{ 5, 1, true },
		{ 4, 0, true },
		{ 12, 1, true },
		{ 0, 0, false },
		{ 1, 4, true },
		{ 2, 6, true },
		{ 7, 4, true }
	};

	constexpr FontAtlasData CreateBitmapFontAtlasData()
	{
		return
		{
			.width = 730.0f,
			.height = 650.0f,
			.leftMargin = 48.0f,
			.topMargin = 48.0f,
			.glyphWidth = 39.0f,
			.glyphHeight = 55.0f,
			.horizontalSpacing = 9.0f,
			.verticalSpacing = 25.0f
		};
	}

	constexpr LabelData CreateVersionLabelData()
	{
		return
		{
			.glyphs = versionLabelGlyphs,
			.glyphCount = static_cast<unsigned int>(sizeof(versionLabelGlyphs) / sizeof(versionLabelGlyphs[0])),
			.height = 0.0375f,
			.rightMargin = 0.05f,
			.bottomMargin = 0.05f
		};
	}

	LabelLayout CreateLabelLayout(const LabelData& label, const FontAtlasData& atlas, float aspectCorrection)
	{
		const float glyphWidth =
			label.height * (atlas.glyphWidth / atlas.glyphHeight) * aspectCorrection;
		const float glyphSpacing = glyphWidth * (atlas.horizontalSpacing / atlas.glyphWidth);
		const float glyphAdvance = glyphWidth + glyphSpacing;
		const float width = label.glyphCount * glyphWidth + (label.glyphCount - 1) * glyphSpacing;
		const float bottom = -1.0f + label.bottomMargin;

		return
		{
			.width = width,
			.left = 1.0f - label.rightMargin - width,
			.bottom = bottom,
			.top = bottom + label.height,
			.glyphWidth = glyphWidth,
			.glyphHeight = label.height,
			.glyphSpacing = glyphSpacing,
			.glyphAdvance = glyphAdvance
		};
	}

	Glyph CreateGlyph(
		const GlyphData& glyphData,
		unsigned int glyphIndex,
		const FontAtlasData& atlas,
		const LabelLayout& label)
	{
		const float left = label.left + glyphIndex * label.glyphAdvance;
		const float textureLeft =
			atlas.leftMargin + glyphData.column * (atlas.glyphWidth + atlas.horizontalSpacing);
		const float textureTop =
			atlas.topMargin + glyphData.row * (atlas.glyphHeight + atlas.verticalSpacing);

		return
		{
			.left = left,
			.right = left + label.glyphWidth,
			.bottom = label.bottom,
			.top = label.top,
			.textureLeft = textureLeft,
			.textureRight = textureLeft + atlas.glyphWidth,
			.textureBottom = textureTop + atlas.glyphHeight,
			.textureTop = textureTop
		};
	}

	void AppendGlyphQuad(
		const Glyph& glyph,
		const FontAtlasData& fontAtlas,
		std::vector<float>& positions,
		std::vector<float>& colors,
		std::vector<float>& textureCoordinates)
	{
		const float glyphPositions[] =
		{
			glyph.left,  glyph.bottom, 0.0f, 1.0f,
			glyph.right, glyph.bottom, 0.0f, 1.0f,
			glyph.left,  glyph.top,    0.0f, 1.0f,

			glyph.right, glyph.bottom, 0.0f, 1.0f,
			glyph.right, glyph.top,    0.0f, 1.0f,
			glyph.left,  glyph.top,    0.0f, 1.0f
		};
		const float glyphTextureCoordinates[] =
		{
			glyph.textureLeft / fontAtlas.width, glyph.textureBottom / fontAtlas.height,
			glyph.textureRight / fontAtlas.width, glyph.textureBottom / fontAtlas.height,
			glyph.textureLeft / fontAtlas.width, glyph.textureTop / fontAtlas.height,

			glyph.textureRight / fontAtlas.width, glyph.textureBottom / fontAtlas.height,
			glyph.textureRight / fontAtlas.width, glyph.textureTop / fontAtlas.height,
			glyph.textureLeft / fontAtlas.width, glyph.textureTop / fontAtlas.height
		};

		for (unsigned int valueIndex = 0;
			valueIndex < verticesPerGlyph * positionValuesPerVertex;
			++valueIndex)
		{
			positions.push_back(glyphPositions[valueIndex]);
			colors.push_back(1.0f);
		}

		for (unsigned int valueIndex = 0;
			valueIndex < verticesPerGlyph * textureCoordinateValuesPerVertex;
			++valueIndex)
		{
			textureCoordinates.push_back(glyphTextureCoordinates[valueIndex]);
		}
	}

	LabelMeshData CreateLabelMeshData(
		const LabelData& labelData,
		const FontAtlasData& fontAtlas,
		const LabelLayout& labelLayout)
	{
		LabelMeshData meshData;
		meshData.positions.reserve(labelData.glyphCount * verticesPerGlyph * positionValuesPerVertex);
		meshData.colors.reserve(labelData.glyphCount * verticesPerGlyph * colorValuesPerVertex);
		meshData.textureCoordinates.reserve(
			labelData.glyphCount * verticesPerGlyph * textureCoordinateValuesPerVertex);

		for (unsigned int glyphIndex = 0; glyphIndex < labelData.glyphCount; ++glyphIndex)
		{
			const GlyphData& glyphData = labelData.glyphs[glyphIndex];
			if (!glyphData.isVisible)
			{
				continue;
			}

			const Glyph glyph = CreateGlyph(glyphData, glyphIndex, fontAtlas, labelLayout);
			AppendGlyphQuad(
				glyph,
				fontAtlas,
				meshData.positions,
				meshData.colors,
				meshData.textureCoordinates);
		}

		return meshData;
	}

	HVertexBuffer CreateLabelVertexBuffer(
		unsigned int floatsPerVertex,
		const CHAR* name,
		std::vector<float>& data)
	{
		VertexBufferCreateDesc desc;
		desc.numOfVertices = static_cast<unsigned int>(data.size() / floatsPerVertex);
		desc.numOfFloatsPerVertex = floatsPerVertex;
		desc.name = name;

		return renderContext.CreateVertexBuffer(desc, data.data());
	}
}

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
	const HTexture outputTexture = renderContext.GetTexture(renderTarget);
	const D3D12_RESOURCE_DESC outputDesc =
		renderContext.GetTexture(outputTexture)->GetResource()->GetDesc();
	const float renderTargetAspectCorrection =
		static_cast<float>(outputDesc.Height) / static_cast<float>(outputDesc.Width);

	const FontAtlasData fontAtlas = CreateBitmapFontAtlasData();
	const LabelData labelData = CreateVersionLabelData();
	const LabelLayout labelLayout =
		CreateLabelLayout(labelData, fontAtlas, renderTargetAspectCorrection);
	LabelMeshData labelMeshData =
		CreateLabelMeshData(labelData, fontAtlas, labelLayout);

	HVertexBuffer position = CreateLabelVertexBuffer(positionValuesPerVertex,
		"UI_Label_Position", labelMeshData.positions);
	HVertexBuffer color = CreateLabelVertexBuffer(colorValuesPerVertex,
		"UI_Label_Color", labelMeshData.colors);
	HVertexBuffer textureCoordinate = CreateLabelVertexBuffer(textureCoordinateValuesPerVertex,
		"UI_Label_TextureCoordinate", labelMeshData.textureCoordinates);

	letterMesh = renderContext.CreateMesh(position, color, textureCoordinate, HVertexBuffer::Invalid(), "UI_Label_Mesh");
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
