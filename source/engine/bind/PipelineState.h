#pragma once

#include <d3d12.h>

class DeviceContext;

class PipelineState
{
public:
	void Create(const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc, ID3D12RootSignature* rootSignature,
		const D3D12_SHADER_BYTECODE& vertexShader, const D3D12_SHADER_BYTECODE& pixelShader, DXGI_FORMAT renderTargetFrormat);
	ID3D12PipelineState* GetPipelineState() { return pipelineState; }
private:
	ID3D12PipelineState* pipelineState;
};