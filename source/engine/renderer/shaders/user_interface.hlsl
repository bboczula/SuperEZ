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
    return BitmapFont.Sample(FontSampler, input.textureCoordinate);
}
