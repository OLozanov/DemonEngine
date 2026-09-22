
static const uint InvalidImage = 0xFFFFFFFF;

static const uint DistanceFunc = 1;
static const uint NormalRange = 2;

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
	uint img_handle;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float3 tcoord : TEXCOORD;
	float4 color : COLOR;
};

Texture2D image[] : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float2 position : POSITION, float3 tcoord : TEXCOORD, float4 color : COLOR)
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
    float4 color = input.color;
	
	float u;
	float v = input.tcoord.y;
	
    if (flags & DistanceFunc)
	{
		u = length(input.tcoord.xz); 
		if (flags & NormalRange) u = u * 2.0 - 1.0;
		
        color.w *= 1.0 - max(0.0, (abs(u) - param)) / (1.0 - param);
	}
	else u = input.tcoord.x;
	
	if (img_handle != InvalidImage) 
		color *= image[img_handle].Sample(g_sampler, float2(u, v));

    return color;
}