Texture2D InputTexture : register(t0); // SRV
RWTexture2D<float4> OutColor : register(u0); // UAV

[numthreads(8, 8, 1)]
void CSMain(uint3 dtid : SV_DispatchThreadID)
{
    float4 color = InputTexture.Load(int3(dtid.xy, 0));
    // Compute luminance (grayscale) using Rec. 601 luma coefficients
    float luma = dot(color.rgb, float3(0.299f, 0.587f, 0.114f));
    float4 grayscale = float4(luma, luma, luma, color.a);
    OutColor[dtid.xy] = grayscale;
}