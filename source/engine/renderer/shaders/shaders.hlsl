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

cbuffer DebugSettings : register(b4)
{
    float forceMipLevel;
    int mipMode;
    float shaderMipBias;
    int visualizeSelectedMip;
    float mipVisualizationStrength;
};

cbuffer CameraPositionData : register(b5)
{
    float4 cameraPosition;
};

cbuffer MaterialData : register(b6)
{
    float materialDiffuseStrength;
    float materialSpecularStrength;
    float materialShininess;
    float materialPadding;
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

float4 SampleDebugTexture(Texture2D textureToSample, SamplerState samplerToUse, float2 uv)
{
    if (mipMode == 1)
    {
        return textureToSample.SampleBias(samplerToUse, uv, shaderMipBias);
    }

    if (mipMode == 2)
    {
        return textureToSample.SampleLevel(samplerToUse, uv, forceMipLevel);
    }

    return textureToSample.Sample(samplerToUse, uv);
}

float GetSelectedMipLevel(Texture2D textureToSample, SamplerState samplerToUse, float2 uv)
{
    if (mipMode == 2)
    {
        return forceMipLevel;
    }

    float lod = textureToSample.CalculateLevelOfDetail(samplerToUse, uv);
    if (mipMode == 1)
    {
        lod += shaderMipBias;
    }

    return lod;
}

float3 GetMipDebugColorByIndex(int mip)
{
    if (mip == 0) return float3(1.0f, 0.0f, 0.0f);
    if (mip == 1) return float3(1.0f, 0.5f, 0.0f);
    if (mip == 2) return float3(1.0f, 1.0f, 0.0f);
    if (mip == 3) return float3(0.0f, 1.0f, 0.0f);
    if (mip == 4) return float3(0.0f, 1.0f, 1.0f);
    if (mip == 5) return float3(0.0f, 0.25f, 1.0f);
    if (mip == 6) return float3(0.75f, 0.0f, 1.0f);
    return float3(1.0f, 0.0f, 1.0f);
}

float3 GetMipDebugColor(float mipLevel)
{
    float clampedMip = clamp(mipLevel, 0.0f, 7.0f);
    int mip0 = (int)floor(clampedMip);
    int mip1 = min(mip0 + 1, 7);
    float mipBlend = frac(clampedMip);

    return lerp(GetMipDebugColorByIndex(mip0), GetMipDebugColorByIndex(mip1), mipBlend);
}

float3 GetTintedMipDebugColor(float3 albedo, float mipLevel)
{
    float3 mipColor = GetMipDebugColor(mipLevel);
    float luminance = dot(albedo, float3(0.2126f, 0.7152f, 0.0722f));
    float3 tintedDetail = mipColor * lerp(0.25f, 1.0f, luminance);

    return lerp(albedo, tintedDetail, saturate(mipVisualizationStrength));
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 uv = float2(input.texCoord.x, 1.0f - input.texCoord.y);
    float4 albedo = SampleDebugTexture(myTexture, LinearSampler, uv);

    if (visualizeSelectedMip != 0)
    {
        return float4(GetTintedMipDebugColor(albedo.rgb, GetSelectedMipLevel(myTexture, LinearSampler, uv)), albedo.a);
    }

    float3 normal = normalize(input.worldNormal);
    float3 lightDir = -normalize(lightDirection.xyz);
    float3 viewDir = normalize(cameraPosition.xyz - input.worldPosition);
    float3 halfVector = normalize(lightDir + viewDir);

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

    float diffuse = lightFacing * diffuseStrength * materialDiffuseStrength;
    float specular = pow(saturate(dot(normal, halfVector)), materialShininess);
    specular *= materialSpecularStrength * step(0.00001f, lightFacing);

    float3 ambientLight = albedo.rgb * lightColor.xyz * ambientStrength;
    float3 directLight = albedo.rgb * lightColor.xyz * diffuse * shadowVisibility;
    float3 specularLight = lightColor.xyz * specular * shadowVisibility;
    return float4(ambientLight + directLight + specularLight, albedo.a);
}
