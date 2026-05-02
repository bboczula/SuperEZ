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

    depthBuffer = renderContext.CreateDepthBuffer(ShadowMapSize, ShadowMapSize, "DB_ShadowMap");
    shadowMapTexture = renderContext.GetTexture(depthBuffer);
    lightViewProjectionBuffer = renderContext.CreateConsantBuffer<SunlightViewProjection>();
    renderContext.UpdateSunlightViewProjection();
    const SunlightViewProjection& lightViewProjection = renderContext.GetSunlightViewProjection();
    renderContext.UpdateConstantBuffer(lightViewProjectionBuffer, &lightViewProjection, sizeof(lightViewProjection));

    // Temporary color target until the shadow pass gets a depth-only PSO.
    renderTarget = renderContext.CreateRenderTarget("RT_ShadowMapPass", RenderTargetFormat::RGB8_UNORM, ShadowMapSize, ShadowMapSize);
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
    renderContext.UpdateSunlightViewProjection();
    const SunlightViewProjection& lightViewProjection = renderContext.GetSunlightViewProjection();
    renderContext.UpdateConstantBuffer(lightViewProjectionBuffer, &lightViewProjection, sizeof(lightViewProjection));
}

void ShadowMapPass::PostSubmit()
{
}

void ShadowMapPass::Allocate(DeviceContext* deviceContext)
{
}
