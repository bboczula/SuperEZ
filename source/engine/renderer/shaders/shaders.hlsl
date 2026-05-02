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

cbuffer SunlightData : register(b2)
{
    float4 lightDirection;
    float4 lightColor;
    float ambientStrength;
    float diffuseStrength;
};

struct VSInput
{
    float4 position : POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float4 normal : NORMAL;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float4 color : COLOR;
    float2 texCoord : TEXCOORD;
    float3 worldNormal : TEXCOORD1;
};

PSInput VSMain(VSInput input)
{
    PSInput o;

    float4 p = float4(input.position.xyz, 1.0f);
    float4 worldPos = mul(p, world);
    o.position = mul(worldPos, viewProjection);

    o.color = input.color;
    o.texCoord = input.texCoord;
    o.worldNormal = normalize(mul(input.normal.xyz, (float3x3)world));
    return o;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 uv = float2(input.texCoord.x, 1.0f - input.texCoord.y);
    float4 albedo = myTexture.Sample(LinearSampler, uv);

    float3 normal = normalize(input.worldNormal);

    float diffuse = saturate(dot(normal, -normalize(lightDirection.xyz))) * diffuseStrength;

    float3 lighting = lightColor.xyz * (ambientStrength + diffuse);
    return float4(albedo.rgb * lighting, albedo.a);
}
