#pragma once

#include <Windows.h>
#include <d3d12.h>

#include <cstdint>
#include <vector>

struct CpuTextureData
{
	UINT width = 0;
	UINT height = 0;
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	std::vector<uint8_t> pixels;
};

struct CpuMipChain
{
	DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM;
	std::vector<CpuTextureData> mips;
};

class TextureGenerator
{
public:
	static CpuTextureData FromRgb8(UINT width, UINT height, const uint8_t* data);
	static CpuTextureData SolidColor(UINT width, UINT height, uint32_t rgba);
	static CpuTextureData Checkerboard(UINT width, UINT height, UINT tileSize, uint32_t colorA, uint32_t colorB);
	static CpuTextureData HorizontalGradient(UINT width, UINT height, uint32_t left, uint32_t right);

	static CpuMipChain GenerateMipChain(const CpuTextureData& base);
	static CpuMipChain DebugMipColors(UINT width, UINT height);

private:
	static CpuTextureData GenerateNextMip(const CpuTextureData& source);
};
