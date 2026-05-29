SamplerState LinearSampler : register(s0);
Texture2D myTexture : register(t0);
Texture2D shadowMap : register(t1);

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
    float shadowBias;
    float shadowSlopeBias;
};

cbuffer LightViewProjectionData : register(b3)
{
    row_major float4x4 lightViewProjection;
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
    float3 worldPosition : TEXCOORD2;
    float4 shadowPosition : TEXCOORD3;
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
    o.worldPosition = worldPos.xyz;
    o.shadowPosition = mul(worldPos, lightViewProjection);
    return o;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 uv = float2(input.texCoord.x, 1.0f - input.texCoord.y);
    float4 albedo = myTexture.Sample(LinearSampler, uv);

    float3 normal = normalize(input.worldNormal);
    float3 shadowNdc = input.shadowPosition.xyz / input.shadowPosition.w;
    float2 shadowUv = shadowNdc.xy * float2(0.5f, -0.5f) + 0.5f;
    float pixelLightDepth = shadowNdc.z;
    float lightFacing = saturate(dot(normal, -normalize(lightDirection.xyz)));
    float depthBias = max(shadowBias, (1.0f - lightFacing) * shadowSlopeBias);
    float shadowMapDepth = shadowMap.Sample(LinearSampler, shadowUv).r;
    bool insideShadowMap =
        shadowUv.x >= 0.0f && shadowUv.x <= 1.0f &&
        shadowUv.y >= 0.0f && shadowUv.y <= 1.0f &&
        pixelLightDepth >= 0.0f && pixelLightDepth <= 1.0f;
    float shadowVisibility = (!insideShadowMap || pixelLightDepth <= shadowMapDepth + depthBias) ? 1.0f : 0.0f;

    float diffuse = lightFacing * diffuseStrength;
    float3 ambientLight = lightColor.xyz * ambientStrength;
    float3 directLight = lightColor.xyz * diffuse * shadowVisibility;
    float3 lighting = ambientLight + directLight;
    return float4(albedo.rgb * lighting, albedo.a);
}
