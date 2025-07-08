#include "RenderTarget.h"
#include "RenderContext.h"
#include "DeviceContext.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderTarget::RenderTarget(UINT width, UINT height, size_t textureIndex, size_t rtvDescriptorIndex, const char* name, DXGI_FORMAT format)
	: width(width), height(height), textureIndex(textureIndex), rtvDescriptorIndex(rtvDescriptorIndex), format(format)
{
	strcpy_s(this->name, name);
}

size_t RenderTarget::GetDescriptorIndex()
{
	return rtvDescriptorIndex;
}
