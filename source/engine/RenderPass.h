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
	size_t shaderIndex;
	size_t rootSignatureIndex;
	size_t pipelineStateIndex;
	size_t viewportAndScissorsIndex;
	size_t commandListIndex;
	size_t renderTargetIndex;
	size_t depthBufferIndex;
	LPCWSTR shaderSourceFileName;
	PCWSTR name;
};