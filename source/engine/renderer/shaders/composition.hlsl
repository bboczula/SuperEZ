static const uint INF = 0xFFFFFFFFu;

Texture2D<float4> InColor : register(t0);
Texture2D<uint> InDist : register(t1);
RWTexture2D<float4> OutColor : register(u0);

cbuffer WidthCB : register(b0)
{
    uint Width;
};

cbuffer HeightCB : register(b1)
{
    uint Height;
};

float AlphaFromD(uint d, uint maxD, float minAlpha)
{
    // d==1 -> 1.0
    // d==maxD -> minAlpha
    float t = (float(maxD) - float(d)) / max(1.0, float(maxD - 1)); // 1 at d=1, 0 at d=maxD
    t = saturate(t);

    // Linear interpolation between minAlpha and 1.0
    return lerp(minAlpha, 1.0, t);
}

[numthreads(8, 8, 1)]
void CSMain(uint3 dtid : SV_DispatchThreadID)
{
    uint2 p = dtid.xy;
    if (p.x >= Width || p.y >= Height)
        return;

    float4 base = InColor.Load(int3(p, 0));
    uint d = InDist.Load(int3(p, 0));

    // Default: unchanged
    float3 outRgb = base.rgb;

    // Only affect pixels that are in the propagated bands (d>0 and not INF)
    if (d != INF && d > 0)
    {
        float MaxD = 8.0; // max distance for blending
        float MinAlpha = 0.0; // minimum blend strength
        float3 HighlightColor = float3(1.0, 0.0, 0.0); // yellow highlight
        float a = AlphaFromD(d, MaxD, MinAlpha); // distance-driven blend strength
        outRgb = lerp(base.rgb, HighlightColor, a);
    }

    OutColor[p] = float4(outRgb, base.a);
}