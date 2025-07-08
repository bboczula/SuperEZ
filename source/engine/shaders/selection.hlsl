//*********************************************************
//
// Copyright (c) Microsoft. All rights reserved.
// This code is licensed under the MIT License (MIT).
// THIS CODE IS PROVIDED *AS IS* WITHOUT WARRANTY OF
// ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING ANY
// IMPLIED WARRANTIES OF FITNESS FOR A PARTICULAR
// PURPOSE, MERCHANTABILITY, OR NON-INFRINGEMENT.
//
//*********************************************************
SamplerState LinearSampler : register(s0);

Texture2D myTexture : register(t0);

cbuffer CameraData : register(b0)
{
	float4x4 viewProjection;
};

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;

    result.position = mul(position, viewProjection);

    return result;
}

uint PSMain(PSInput input) : SV_TARGET
{
    // Render Target is R32_UINT
    return uint(1);
}