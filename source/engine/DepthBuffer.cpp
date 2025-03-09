#include "DepthBuffer.h"
#include "RenderContext.h"
#include "DeviceContext.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

DepthBuffer::DepthBuffer(UINT width, UINT height, size_t textureIndex, size_t dsvDescriptorIndex, const char* name)
	: width(width), height(height), textureIndex(textureIndex), dsvDescriptorIndex(dsvDescriptorIndex)
{
	strcpy_s(this->name, name);
}

size_t DepthBuffer::GetDescriptorIndex()
{
	return dsvDescriptorIndex;
}
