#pragma once

#include <d3d12.h>

class RenderTarget;
class DeviceContext;
class RenderContext;

class RenderPass
{
public:
	enum Type
	{
		Default,
		Drawless
	};
	RenderPass(PCWSTR name, RenderPass::Type type);
	~RenderPass();
	void AutomaticPrepare();
	virtual void Prepare();
	virtual void Update();
	void PreExecute();
	virtual void Execute() = 0;
	void PostExecute();
	virtual void Allocate(DeviceContext* deviceContext) = 0;
	RenderPass::Type GetType() const { return type; }
protected:
	RenderPass::Type type;
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