#pragma once

#include <d3d12.h>
#include <wrl/client.h>
#include "d3dx12.h"
#include <string>

class DeviceContext;

class RootSignatureBuilder {
public:
	void AddConstants(UINT num32BitValues, UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY visibility);
	void AddCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility);
	void AddSRVTable(UINT baseRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);
	void AddSamplerTable(UINT baseRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility);

	ID3D12RootSignature* Build(DeviceContext* deviceContext, const std::wstring& name);

private:
	static constexpr UINT MaxParams = 16;
	CD3DX12_ROOT_PARAMETER m_rootParams[MaxParams];
	CD3DX12_DESCRIPTOR_RANGE m_ranges[MaxParams];
	UINT m_paramCount = 0;
	UINT m_rangeCount = 0;
};
