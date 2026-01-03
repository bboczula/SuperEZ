#pragma once

#include <d3d12.h>

class DeviceContext;

class PipelineState
{
public:
	enum Type
	{
		Uninitialized,
		Graphics,
		Compute
	};
	void Create(const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc, ID3D12RootSignature* rootSignature,
		const D3D12_SHADER_BYTECODE& vertexShader, const D3D12_SHADER_BYTECODE& pixelShader, DXGI_FORMAT renderTargetFrormat);
	void Create(ID3D12RootSignature* rootSignature, const D3D12_SHADER_BYTECODE& computeShader);
	ID3D12PipelineState* Get() { return pipelineState; }
	bool IsGrpahics() const { return type == Type::Graphics; }
	bool IsCompute() const { return type == Type::Compute; }
	bool IsValid() const { return type != Type::Uninitialized && pipelineState != nullptr; }
private:
	ID3D12PipelineState* pipelineState;
	Type type = Type::Uninitialized;
};