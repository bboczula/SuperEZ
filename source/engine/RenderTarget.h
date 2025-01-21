#include "Texture.h"

class RenderTarget
{
public:
	RenderTarget(UINT width, UINT height, UINT textureIndex, UINT rtvDescriptorIndex, const char* name);
	UINT GetDescriptorIndex();
private:
	char name[32];
	UINT textureIndex;
	UINT rtvDescriptorIndex;
	UINT width;
	UINT height;
};