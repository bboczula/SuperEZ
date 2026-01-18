RWTexture2D<uint> gSeeds : register(u0);

cbuffer WidthCB : register(b0)
{
    uint width;
};

cbuffer HeightCB : register(b1)
{
    uint height;
};

static const uint INVALID = 0xFFFFFFFFu;

[numthreads(8, 8, 1)]
void CSMain(uint3 dtid : SV_DispatchThreadID)
{
    if (dtid.x >= width || dtid.y >= height)
        return;

    gSeeds[uint2(dtid.xy)] = uint(INVALID);
}
