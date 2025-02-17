#pragma once

#include <d3d12.h>

class RenderTarget;
class DeviceContext;
class RenderContext;

class RenderPass
{
public:
	RenderPass(PCWSTR name);
	~RenderPass();
	void AutomaticPrepare();
	virtual void Prepare();
	virtual void Update();
	void PreExecute();
	virtual void Execute() = 0;
	void PostExecute();
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
	PCWSTR name;
};