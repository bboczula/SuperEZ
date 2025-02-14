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
	void AutomaticPrepare();
	virtual void Prepare();
	virtual void Update();
	virtual void Execute() = 0;
	virtual void Allocate(DeviceContext* deviceContext) = 0;
protected:
	UINT shaderIndex;
	UINT rootSignatureIndex;
	UINT pipelineStateIndex;
	UINT viewportAndScissorsIndex;
	UINT commandListIndex;
	UINT renderTargetIndex;
	UINT depthBufferIndex;
	LPCWSTR shaderSourceFileName;
};