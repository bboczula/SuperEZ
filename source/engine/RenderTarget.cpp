#include "RenderTarget.h"
#include "RenderContext.h"
#include "DeviceContext.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderTarget::RenderTarget(UINT width, UINT height, UINT textureIndex, UINT rtvDescriptorIndex, const char* name)
	: width(width), height(height), textureIndex(textureIndex), rtvDescriptorIndex(rtvDescriptorIndex)
{
	strcpy_s(this->name, name);
}

UINT RenderTarget::GetDescriptorIndex()
{
	return rtvDescriptorIndex;
}
