
cbuffer ScreenConstantBuffer : register(b0)
{
    float4 screenMat[2];
	float padding[58];
};

cbuffer DrawConstantBuffer : register(b1)
{
    float2 offset;
    uint flags;
    float param;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
	float4 color : COLOR;
};

Texture2D image : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float2 position : POSITION, float2 tcoord : TEXCOORD, float4 color : COLOR)
{
    PSInput result;

    float2 pos = screenMat[0].xy*(position.x + offset.x) + screenMat[0].zw*(position.y + offset.y) + screenMat[1].xy;
    
    result.position = float4(pos, 0.0, 1.0);
    result.tcoord = tcoord;
    result.color = color;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = image.Sample(g_sampler, input.tcoord);

    if (flags & 1)
        color.w *= 1.0 - max(0.0, (abs(input.tcoord.x) - param)) / (1.0 - param);
    
    return color*input.color;
}