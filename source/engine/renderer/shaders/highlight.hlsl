static const uint INF = 0xFFFFFFFFu;

Texture2D<uint> InDist : register(t0);
RWTexture2D<uint> OutDist : register(u0);

cbuffer WidthCB : register(b0)
{
    uint Width;
};

cbuffer HeightCB : register(b1)
{
    uint Height;
};

[numthreads(8, 8, 1)]
void CSMain(uint3 tid : SV_DispatchThreadID)
{
    uint2 p = tid.xy;
    if (p.x >= Width || p.y >= Height)
        return;

    uint best = InDist[p];

    int2 offsets[4] =
    {
        int2(-1, 0),
        int2(1, 0),
        int2(0, -1),
        int2(0, 1)
    };

    for (int i = 0; i < 4; i++)
    {
        int2 q = int2(p) + offsets[i];
        if (q.x < 0 || q.y < 0 || q.x >= int(Width) || q.y >= int(Height))
            continue;

        uint nd = InDist[uint2(q)];
        if (nd != INF)
            best = min(best, nd + 1);
    }

    OutDist[p] = best;
}
