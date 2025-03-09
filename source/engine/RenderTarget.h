#include "Texture.h"

class RenderTarget
{
public:
	RenderTarget(UINT width, UINT height, size_t textureIndex, size_t rtvDescriptorIndex, const char* name);
	size_t GetDescriptorIndex();
private:
	char name[32];
	size_t textureIndex;
	size_t rtvDescriptorIndex;
	UINT width;
	UINT height;
};