SamplerState LinearSampler : register(s0);
Texture2D myTexture : register(t0);

cbuffer CameraData : register(b0)
{
    row_major float4x4 viewProjection;
};

cbuffer ObjectData : register(b1)
{
    row_major float4x4 world;
};

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
};

PSInput VSMain(VSInput input)
{
    PSInput o;

    float4 p = float4(input.position.xyz, 1.0f);
    float4 worldPos = mul(p, world);
    o.position = mul(worldPos, viewProjection);

    o.color = input.color;
    o.texCoord = input.texCoord;
    return o;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 uv = float2(input.texCoord.x, 1.0f - input.texCoord.y);
    return myTexture.Sample(LinearSampler, uv);
}
