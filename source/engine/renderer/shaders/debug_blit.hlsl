// Debug render target viewer.
// Decodes and letterbox-scales an arbitrary source texture into the output,
// which BlitPass then copies to the back buffer.

#define MODE_COLOR 0
#define MODE_UINT_ID 1
#define MODE_DEPTH 2
// RenderTargetFormat::R32_UINT actually creates an R32_FLOAT resource; passes
// like Selection write raw uint bits into it, so recover the ID with asuint().
#define MODE_UINT_BITCAST 3

cbuffer DebugBlitConstants : register(b0)
{
    uint Mode;
};

RWTexture2D<float4> OutColor : register(u0); // UAV

// The same source texture is bound to both slots; only the one matching
// Mode is ever read, so the type mismatch on the other slot is harmless.
Texture2D<float4> SourceFloat : register(t0); // SRV (color / depth)
Texture2D<uint> SourceUint : register(t1);    // SRV (object IDs etc.)

float3 IdToColor(uint id)
{
    if (id == 0u || id == 0xFFFFFFFFu)
    {
        return float3(0.0f, 0.0f, 0.0f);
    }

    return float3(
        ((id * 97u + 13u) % 255u) / 255.0f,
        ((id * 57u + 41u) % 255u) / 255.0f,
        ((id * 33u + 71u) % 255u) / 255.0f);
}

[numthreads(8, 8, 1)]
void CSMain(uint3 dtid : SV_DispatchThreadID)
{
    uint dstWidth, dstHeight;
    OutColor.GetDimensions(dstWidth, dstHeight);
    if (dtid.x >= dstWidth || dtid.y >= dstHeight)
    {
        return;
    }

    uint srcWidth, srcHeight;
    if (Mode == MODE_UINT_ID)
    {
        SourceUint.GetDimensions(srcWidth, srcHeight);
    }
    else
    {
        SourceFloat.GetDimensions(srcWidth, srcHeight);
    }

    // Letterbox: uniform scale, centered, dark gray outside the source.
    float scale = min((float)dstWidth / (float)srcWidth, (float)dstHeight / (float)srcHeight);
    float2 scaledSize = float2(srcWidth, srcHeight) * scale;
    float2 offset = (float2(dstWidth, dstHeight) - scaledSize) * 0.5f;
    float2 srcPos = (float2(dtid.xy) - offset) / scale;

    if (srcPos.x < 0.0f || srcPos.y < 0.0f || srcPos.x >= (float)srcWidth || srcPos.y >= (float)srcHeight)
    {
        OutColor[dtid.xy] = float4(0.1f, 0.1f, 0.1f, 1.0f);
        return;
    }

    int3 loadPos = int3(int2(srcPos), 0);

    float4 result;
    if (Mode == MODE_UINT_ID)
    {
        result = float4(IdToColor(SourceUint.Load(loadPos)), 1.0f);
    }
    else if (Mode == MODE_DEPTH)
    {
        float depth = SourceFloat.Load(loadPos).r;
        result = float4(depth, depth, depth, 1.0f);
    }
    else if (Mode == MODE_UINT_BITCAST)
    {
        result = float4(IdToColor(asuint(SourceFloat.Load(loadPos).r)), 1.0f);
    }
    else // MODE_COLOR
    {
        result = float4(SourceFloat.Load(loadPos).rgb, 1.0f);
    }

    OutColor[dtid.xy] = result;
}
