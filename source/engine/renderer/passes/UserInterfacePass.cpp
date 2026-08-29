#include "UserInterfacePass.h"
#include "../RenderContext.h"
#include "../../bind/RootSignatureBuilder.h"
#include "../../core/InputLayout.h"

extern RenderContext renderContext;

UserInterfacePass::UserInterfacePass() : RenderPass(L"UserInterface", L"user_interface.hlsl", Type::Graphics)
{
}

UserInterfacePass::~UserInterfacePass()
{
}

void UserInterfacePass::ConfigurePipelineState()
{
    // Pre-AutomaticInitialize Procedure
    inputLayout = renderContext.CreateInputLayout();
    renderContext.GetInputLayout(inputLayout)->AppendElementT(VertexStream::Position);

    // Now we can create the root signature
    RootSignatureBuilder builder;
    rootSignature = renderContext.CreateRootSignature(builder);

    renderTarget = renderContext.CreateRenderTarget("RT_UserInterfacePass", RenderTargetFormat::RGB8_UNORM, 64, 64);
}

void UserInterfacePass::PostAssetLoad()
{
}

void UserInterfacePass::Initialize()
{
	float quad[] = { 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f };
	renderContext.CreateVertexBuffer(4, 2, quad, "UI_Letter");
}

void UserInterfacePass::Update()
{
}

void UserInterfacePass::Execute()
{
}

void UserInterfacePass::PostSubmit()
{
}

void UserInterfacePass::Allocate(DeviceContext* deviceContext)
{
}
