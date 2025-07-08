#include "Texture.h"

class RenderTarget
{
public:
	RenderTarget(UINT width, UINT height, size_t textureIndex, size_t rtvDescriptorIndex, const char* name, DXGI_FORMAT format);
	size_t GetDescriptorIndex();
	const char* GetName() const { return name; }
	UINT GetWidth() const { return width; }
	UINT GetHeight() const { return height; }
	DXGI_FORMAT GetFormat() const { return format; }
private:
	char name[32];
	size_t textureIndex;
	size_t rtvDescriptorIndex;
	UINT width;
	UINT height;
	DXGI_FORMAT format;
};