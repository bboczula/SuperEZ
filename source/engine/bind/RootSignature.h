#pragma once

#include <d3d12.h>

#include "RootSignatureBuilder.h"

class DeviceContext;

class RootSignature
{
public:
	void Create(RootSignatureBuilder& builder);
	ID3D12RootSignature* GetRootSignature() { return rootSignature; }
private:
	ID3D12RootSignature* rootSignature;
};