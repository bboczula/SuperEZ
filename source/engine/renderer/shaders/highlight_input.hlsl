cbuffer CameraData : register(b0)
{
    float4x4 viewProjection;
};

cbuffer ObjectData : register(b1)
{
    float4x4 world;
};

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
    return 0;
}
