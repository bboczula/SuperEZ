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

cbuffer ObjectData : register(b1)
{
    float4x4 world;
};

uint objectID : register(b2); // One 32-bit constant

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;

    float4 p = float4(position.xyz, 1.0f); // <-- guaranteed w=1
    float4 worldPos = mul(p, world);
    result.position = mul(worldPos, viewProjection);

    return result;
}

uint PSMain(PSInput input) : SV_TARGET
{
    // Render Target is R32_UINT
    return objectID;
}