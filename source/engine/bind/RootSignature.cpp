#include "RootSignature.h"
#include "../core/DeviceContext.h"

extern DeviceContext deviceContext;

void RootSignature::Create(RootSignatureBuilder& builder)
{
	rootSignature = builder.Build(&deviceContext, L"RenderContextRootSignature");
	if (!rootSignature)
	{
		OutputDebugString(L"Failed to create root signature\n");
	}
}