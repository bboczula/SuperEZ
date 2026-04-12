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

    // Direction points from the light toward the scene, like sunlight.
    float3 lightDirection = normalize(float3(-0.4f, -1.0f, -0.3f));
    float3 lightColor = float3(1.0f, 0.98f, 0.92f);
    float ambientStrength = 0.2f;
    float diffuseStrength = saturate(dot(normal, -lightDirection));

    float3 lighting = lightColor * (ambientStrength + diffuseStrength);
    return float4(albedo.rgb * lighting, albedo.a);
}
