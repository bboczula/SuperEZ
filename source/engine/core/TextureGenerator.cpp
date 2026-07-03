#include "TextureGenerator.h"

#include <algorithm>
#include <cassert>

namespace
{
	constexpr UINT BytesPerPixel = 4;

	struct RgbaColor
	{
		uint8_t r = 0;
		uint8_t g = 0;
		uint8_t b = 0;
		uint8_t a = 255;
	};

	RgbaColor UnpackRgba(uint32_t rgba)
	{
		return RgbaColor{
			static_cast<uint8_t>((rgba >> 24) & 0xFF),
			static_cast<uint8_t>((rgba >> 16) & 0xFF),
			static_cast<uint8_t>((rgba >> 8) & 0xFF),
			static_cast<uint8_t>(rgba & 0xFF)
		};
	}

	void WritePixel(CpuTextureData& texture, UINT x, UINT y, RgbaColor color)
	{
		const size_t index = (static_cast<size_t>(y) * texture.width + x) * BytesPerPixel;
		texture.pixels[index + 0] = color.r;
		texture.pixels[index + 1] = color.g;
		texture.pixels[index + 2] = color.b;
		texture.pixels[index + 3] = color.a;
	}

	RgbaColor ReadPixel(const CpuTextureData& texture, UINT x, UINT y)
	{
		const size_t index = (static_cast<size_t>(y) * texture.width + x) * BytesPerPixel;
		return RgbaColor{
			texture.pixels[index + 0],
			texture.pixels[index + 1],
			texture.pixels[index + 2],
			texture.pixels[index + 3]
		};
	}

	CpuTextureData CreateTexture(UINT width, UINT height)
	{
		CpuTextureData texture;
		texture.width = width;
		texture.height = height;
		texture.format = DXGI_FORMAT_R8G8B8A8_UNORM;
		texture.pixels.resize(static_cast<size_t>(width) * height * BytesPerPixel);
		return texture;
	}

	UINT MaxUint(UINT a, UINT b)
	{
		return a > b ? a : b;
	}

	RgbaColor Lerp(RgbaColor a, RgbaColor b, float t)
	{
		auto mix = [t](uint8_t from, uint8_t to)
		{
			const float value = static_cast<float>(from) + (static_cast<float>(to) - static_cast<float>(from)) * t;
			return static_cast<uint8_t>(std::clamp(value, 0.0f, 255.0f));
		};

		return RgbaColor{
			mix(a.r, b.r),
			mix(a.g, b.g),
			mix(a.b, b.b),
			mix(a.a, b.a)
		};
	}
}

CpuTextureData TextureGenerator::FromRgb8(UINT width, UINT height, const uint8_t* data)
{
	assert(data != nullptr);

	CpuTextureData texture = CreateTexture(width, height);
	size_t inputIndex = 0;
	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			const RgbaColor color{
				data[inputIndex++],
				data[inputIndex++],
				data[inputIndex++],
				255
			};
			WritePixel(texture, x, y, color);
		}
	}

	return texture;
}

CpuTextureData TextureGenerator::SolidColor(UINT width, UINT height, uint32_t rgba)
{
	CpuTextureData texture = CreateTexture(width, height);
	const RgbaColor color = UnpackRgba(rgba);
	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			WritePixel(texture, x, y, color);
		}
	}

	return texture;
}

CpuTextureData TextureGenerator::Checkerboard(UINT width, UINT height, UINT tileSize, uint32_t colorA, uint32_t colorB)
{
	CpuTextureData texture = CreateTexture(width, height);
	const RgbaColor a = UnpackRgba(colorA);
	const RgbaColor b = UnpackRgba(colorB);
	const UINT safeTileSize = MaxUint(tileSize, 1u);

	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			const bool useA = ((x / safeTileSize) + (y / safeTileSize)) % 2 == 0;
			WritePixel(texture, x, y, useA ? a : b);
		}
	}

	return texture;
}

CpuTextureData TextureGenerator::HorizontalGradient(UINT width, UINT height, uint32_t left, uint32_t right)
{
	CpuTextureData texture = CreateTexture(width, height);
	const RgbaColor leftColor = UnpackRgba(left);
	const RgbaColor rightColor = UnpackRgba(right);
	const float denominator = static_cast<float>(MaxUint(width, 1u) - 1u);

	for (UINT y = 0; y < height; ++y)
	{
		for (UINT x = 0; x < width; ++x)
		{
			const float t = denominator > 0.0f ? static_cast<float>(x) / denominator : 0.0f;
			WritePixel(texture, x, y, Lerp(leftColor, rightColor, t));
		}
	}

	return texture;
}

CpuMipChain TextureGenerator::GenerateMipChain(const CpuTextureData& base)
{
	assert(base.format == DXGI_FORMAT_R8G8B8A8_UNORM);
	assert(base.width > 0 && base.height > 0);

	CpuMipChain chain;
	chain.format = base.format;
	chain.mips.push_back(base);

	while (chain.mips.back().width > 1 || chain.mips.back().height > 1)
	{
		chain.mips.push_back(GenerateNextMip(chain.mips.back()));
	}

	return chain;
}

CpuMipChain TextureGenerator::DebugMipColors(UINT width, UINT height)
{
	static constexpr uint32_t DebugColors[] =
	{
		0xFF4040FF,
		0x40FF40FF,
		0x4080FFFF,
		0xFFD040FF,
		0xB040FFFF,
		0x40FFFFFF,
		0xFF80C0FF,
		0xC0FF40FF
	};

	CpuMipChain chain;
	chain.format = DXGI_FORMAT_R8G8B8A8_UNORM;

	UINT mipWidth = MaxUint(width, 1u);
	UINT mipHeight = MaxUint(height, 1u);
	UINT mipIndex = 0;
	while (true)
	{
		constexpr UINT debugColorCount = static_cast<UINT>(sizeof(DebugColors) / sizeof(DebugColors[0]));
		const uint32_t color = DebugColors[mipIndex % debugColorCount];
		chain.mips.push_back(SolidColor(mipWidth, mipHeight, color));

		if (mipWidth == 1 && mipHeight == 1)
		{
			break;
		}

		mipWidth = MaxUint(mipWidth / 2, 1u);
		mipHeight = MaxUint(mipHeight / 2, 1u);
		++mipIndex;
	}

	return chain;
}

CpuTextureData TextureGenerator::GenerateNextMip(const CpuTextureData& source)
{
	assert(source.format == DXGI_FORMAT_R8G8B8A8_UNORM);

	const UINT nextWidth = MaxUint(source.width / 2, 1u);
	const UINT nextHeight = MaxUint(source.height / 2, 1u);
	CpuTextureData mip = CreateTexture(nextWidth, nextHeight);

	for (UINT y = 0; y < nextHeight; ++y)
	{
		for (UINT x = 0; x < nextWidth; ++x)
		{
			const UINT sourceX0 = static_cast<UINT>((static_cast<UINT64>(x) * source.width) / nextWidth);
			const UINT sourceX1 = static_cast<UINT>((static_cast<UINT64>(x + 1) * source.width) / nextWidth);
			const UINT sourceY0 = static_cast<UINT>((static_cast<UINT64>(y) * source.height) / nextHeight);
			const UINT sourceY1 = static_cast<UINT>((static_cast<UINT64>(y + 1) * source.height) / nextHeight);
			UINT sampleCount = 0;
			UINT r = 0;
			UINT g = 0;
			UINT b = 0;
			UINT a = 0;

			for (UINT sampleY = sourceY0; sampleY < sourceY1; ++sampleY)
			{
				for (UINT sampleX = sourceX0; sampleX < sourceX1; ++sampleX)
				{
					const RgbaColor sample = ReadPixel(source, sampleX, sampleY);
					r += sample.r;
					g += sample.g;
					b += sample.b;
					a += sample.a;
					++sampleCount;
				}
			}

			const RgbaColor averaged{
				static_cast<uint8_t>((r + sampleCount / 2) / sampleCount),
				static_cast<uint8_t>((g + sampleCount / 2) / sampleCount),
				static_cast<uint8_t>((b + sampleCount / 2) / sampleCount),
				static_cast<uint8_t>((a + sampleCount / 2) / sampleCount)
			};
			WritePixel(mip, x, y, averaged);
		}
	}

	return mip;
}
