cbuffer LightData : register(b0)
{
    row_major float4x4 lightViewProjection;
};

cbuffer ObjectData : register(b1)
{
    row_major float4x4 world;
};

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;
    float4 worldPosition = mul(float4(position.xyz, 1.0f), world);
    result.position = mul(worldPosition, lightViewProjection);
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
