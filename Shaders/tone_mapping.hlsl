static const float2 quad[4] = {{ -1.0f, 1.0f },
                               { 1.0f, 1.0f },
                               { -1.0f, -1.0f },
                               { 1.0f, -1.0f }};
                               
Texture2D framebuffer : register(t0);

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(uint vid: SV_VertexID)
{
    PSInput result;
    result.position = float4(quad[vid].x, quad[vid].y, 1.0, 1.0);
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    const float gamma = 2.2;
    const float exposure = 1.0;

    float3 hdrColor = framebuffer.Load(float3(input.position.xy, 0));
    //float3 color = hdrColor/(hdrColor + 1.0);
    float3 color = 1.0 - exp(-hdrColor*exposure);
    color = pow(color, 1.0/gamma);
    
    return float4(color, 1.0);
}