static const uint INF = 0xFFFFFFFFu;

Texture2D<float4> InColor : register(t0);
Texture2D<uint> InHighlight : register(t1);
RWTexture2D<float4> OutDist : register(u0);

cbuffer WidthCB : register(b0)
{
    uint Width;
};

cbuffer HeightCB : register(b1)
{
    uint Height;
};

[numthreads(8, 8, 1)]
void CSMain(uint3 dtid : SV_DispatchThreadID)
{
    uint2 p = uint2(dtid.xy);

    float4 base = InColor.Load(int3(p, 0));
    uint d = InHighlight.Load(int3(p, 0));

    float a = 0.0;

    // ignore INF (non-reached)
    if (d != 0xFFFFFFFFu)
    {
        if (d > 0)
        {
            OutDist[p] = float4(1.0f, 0.0f, 0.0f, 1.0f);
            return;
        }

    }
    OutDist[p] = base;

}
