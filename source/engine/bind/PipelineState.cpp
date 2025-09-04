#include "PipelineState.h"
#include "../core/DeviceContext.h"

#include "d3dx12.h"
#include "../Utils.h"

extern DeviceContext deviceContext;

void PipelineState::Create(const D3D12_INPUT_LAYOUT_DESC& inputLayoutDesc, ID3D12RootSignature* rootSignature,
	const D3D12_SHADER_BYTECODE& vertexShader, const D3D12_SHADER_BYTECODE& pixelShader, DXGI_FORMAT renderTargetFrormat)
{
	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {};
	psoDesc.InputLayout = inputLayoutDesc;
	psoDesc.pRootSignature = rootSignature;
	psoDesc.VS = vertexShader;
	psoDesc.PS = pixelShader;
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
	psoDesc.RasterizerState.CullMode = D3D12_CULL_MODE_FRONT;
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
	psoDesc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
	psoDesc.SampleMask = UINT_MAX;
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	psoDesc.NumRenderTargets = 1;
	psoDesc.RTVFormats[0] = renderTargetFrormat;
	psoDesc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	psoDesc.SampleDesc.Count = 1;

	//ID3D12PipelineState* pipelineState;
	HRESULT hr = deviceContext.GetDevice()->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipelineState));
	DX_TRY(hr, "CreateGraphicsPipelineState", "Did you forget to update your root signature or input layout?");
	pipelineState->SetName(L"Render Context Pipeline State");
};