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

struct PSInput
{
    float4 position : SV_POSITION;
//    float4 color : COLOR;
};

//PSInput VSMain(float4 position : POSITION, float4 color : COLOR)
PSInput VSMain(float4 position : POSITION)
{
    PSInput result;

    result.position = position;
    //result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //return input.color;
	return float4(0.89f, 0.43f, 0.07f, 1.0f);
}