#pragma once

#include <d3d12.h>
#include "../asset/Handle.h"

class RenderTarget;
class DeviceContext;
class RenderContext;

class RenderPass
{
public:
	enum Type
	{
		Default,
		Compute,
		Drawless
	};
	RenderPass(PCWSTR name, LPCWSTR shaderName, RenderPass::Type type);
	~RenderPass();
	virtual void ConfigurePipelineState() = 0;
	void AutomaticInitialize();
	virtual void Initialize();
	virtual void Update();
	void PreExecute();
	virtual void Execute() = 0;
	void PostExecute();
	virtual void Allocate(DeviceContext* deviceContext) = 0;
	RenderPass::Type GetType() const { return type; }
protected:
	RenderPass::Type type;
	HShader vertexShader;
	HShader pixelShader;
	HShader computeShader;
	HRootSignature rootSignature;
	HInputLayout inputLayout;
	HPipelineState pipelineState;
	HViewportAndScissors viewportAndScissors;
	HCommandList commandList;
	HRenderTarget renderTarget;
	HDepthBuffer depthBuffer;
	LPCWSTR shaderSourceFileName;
	PCWSTR name;
};