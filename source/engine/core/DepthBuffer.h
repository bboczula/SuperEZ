#include <Windows.h>

class DepthBuffer
{
public:
	DepthBuffer(UINT width, UINT height, size_t textureIndex, size_t dsvDescriptorIndex, const char* name);
	size_t GetDescriptorIndex();
private:
	char name[32];
	size_t textureIndex;
	size_t dsvDescriptorIndex;
	UINT width;
	UINT height;
};