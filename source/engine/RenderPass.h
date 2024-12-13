#pragma once

#include <d3d12.h>

class RenderTarget;
class DeviceContext;
class RenderContext;

class RenderPass
{
public:
	RenderPass();
	~RenderPass();
	virtual void Prepare();
	virtual void Execute() = 0;
	virtual void Allocate(DeviceContext* deviceContext) = 0;
protected:
	UINT shaderIndex;
};