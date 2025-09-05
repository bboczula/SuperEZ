#include "RootSignatureBuilder.h"
#include "../core/DeviceContext.h"
#include <d3d12.h>
#include <d3dx12.h>
#include <wrl/client.h>
#include <string>

void RootSignatureBuilder::AddConstants(UINT num32BitValues, UINT shaderRegister, UINT registerSpace, D3D12_SHADER_VISIBILITY visibility)
{
      if (m_paramCount >= MaxParams) return;

      m_rootParams[m_paramCount].InitAsConstants(num32BitValues, shaderRegister, registerSpace, visibility);
      ++m_paramCount;
}

void RootSignatureBuilder::AddCBV(UINT shaderRegister, D3D12_SHADER_VISIBILITY visibility)
{
      if (m_paramCount >= MaxParams) return;

      m_rootParams[m_paramCount].InitAsConstantBufferView(shaderRegister, 0, visibility);
      ++m_paramCount;
}

void RootSignatureBuilder::AddSRVTable(UINT baseRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
      if (m_paramCount >= MaxParams || m_rangeCount >= MaxParams) return;

      m_ranges[m_rangeCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, numDescriptors, baseRegister);
      m_rootParams[m_paramCount].InitAsDescriptorTable(1, &m_ranges[m_rangeCount], visibility);

      ++m_rangeCount;
      ++m_paramCount;
}

void RootSignatureBuilder::AddSamplerTable(UINT baseRegister, UINT numDescriptors, D3D12_SHADER_VISIBILITY visibility)
{
      if (m_paramCount >= MaxParams || m_rangeCount >= MaxParams) return;

      m_ranges[m_rangeCount].Init(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, numDescriptors, baseRegister);
      m_rootParams[m_paramCount].InitAsDescriptorTable(1, &m_ranges[m_rangeCount], visibility);

      ++m_rangeCount;
      ++m_paramCount;
}

ID3D12RootSignature* RootSignatureBuilder::Build(DeviceContext* deviceContext, const std::wstring& name)
{
      CD3DX12_ROOT_SIGNATURE_DESC desc;
      desc.Init(m_paramCount, m_rootParams, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

      Microsoft::WRL::ComPtr<ID3DBlob> signature;
      Microsoft::WRL::ComPtr<ID3DBlob> error;
      HRESULT hr = D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
      if (FAILED(hr)) {
            if (error) OutputDebugStringA((char*)error->GetBufferPointer());
            return nullptr;
      }

      ID3D12RootSignature* rootSig = nullptr;
      hr = deviceContext->GetDevice()->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&rootSig));
      if (FAILED(hr)) return nullptr;

      rootSig->SetName(name.c_str());
      return rootSig;
}
