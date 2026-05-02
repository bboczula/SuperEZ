#include "ShadowMapPass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../core/InputLayout.h"

extern RenderContext renderContext;

ShadowMapPass::ShadowMapPass() : RenderPass(L"ShadowMap", L"shadow_map.hlsl", Type::Graphics)
{
}

ShadowMapPass::~ShadowMapPass()
{
}

void ShadowMapPass::ConfigurePipelineState()
{
    // Pre-AutomaticInitialize Procedure
    inputLayout = renderContext.CreateInputLayout();
    renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);

    // Now we can create the root signature
    RootSignatureBuilder builder;
    rootSignature = renderContext.CreateRootSignature(builder);

    renderTarget = renderContext.CreateRenderTarget("RT_ShadowMapPass", RenderTargetFormat::RGB8_UNORM, 64, 64);
}

void ShadowMapPass::PostAssetLoad()
{
}

void ShadowMapPass::Initialize()
{
}

void ShadowMapPass::Update()
{
}

void ShadowMapPass::Execute()
{
}

void ShadowMapPass::PostSubmit()
{
}

void ShadowMapPass::Allocate(DeviceContext* deviceContext)
{
}
