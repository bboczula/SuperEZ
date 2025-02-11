#include <Windows.h>

class DepthBuffer
{
public:
	DepthBuffer(UINT width, UINT height, UINT textureIndex, UINT dsvDescriptorIndex, const char* name);
	UINT GetDescriptorIndex();
private:
	char name[32];
	UINT textureIndex;
	UINT dsvDescriptorIndex;
	UINT width;
	UINT height;
};