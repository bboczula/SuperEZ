#include "RenderTarget.h"
#include "RenderContext.h"
#include "../core/DeviceContext.h"

extern RenderContext renderContext;
extern DeviceContext deviceContext;

RenderTarget::RenderTarget(UINT width, UINT height, size_t textureIndex, size_t rtvDescriptorIndex, const char* name, DXGI_FORMAT format)
	: width(width), height(height), textureIndex(textureIndex), rtvDescriptorIndex(rtvDescriptorIndex), format(format)
{
	strcpy_s(this->name, name);
	viewport = CD3DX12_VIEWPORT(0.0f, 0.0f, static_cast<float>(width), static_cast<float>(height));
	scissorRect = CD3DX12_RECT(0, 0, static_cast<LONG>(width), static_cast<LONG>(height));
}

size_t RenderTarget::GetDescriptorIndex()
{
	return rtvDescriptorIndex;
}
