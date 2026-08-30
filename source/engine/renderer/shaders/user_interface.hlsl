Texture2D BitmapFont : register(t0);
SamplerState FontSampler : register(s0);

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 textureCoordinate : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 textureCoordinate : TEXCOORD;
};

PSInput VSMain(VSInput input)
{
    PSInput result;
    result.position = input.position;
    result.textureCoordinate = input.textureCoordinate;
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = BitmapFont.Sample(FontSampler, input.textureCoordinate);

    if (all(color.rgb < float3(0.1f, 0.1f, 0.1f)))
    {
        discard;
    }

    return float4(0.0f, 0.0f, 0.0f, 1.0f);
}
