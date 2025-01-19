#include "RenderTarget.h"
#include "RenderContext.h"
#include "DeviceContext.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderTarget::RenderTarget(UINT width, UINT height, UINT textureIndex, UINT rtvDescriptorIndex)
	: width(width), height(height), textureIndex(textureIndex), rtvDescriptorIndex(rtvDescriptorIndex)
{
}

UINT RenderTarget::GetDescriptorIndex()
{
	return rtvDescriptorIndex;
}
