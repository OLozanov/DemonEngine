cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float padding[48];
};

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelMat;
};

cbuffer ScreenConstantBuffer : register(b2)
{
    uint width;
    uint height;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
};

Texture2D diffuse_map : register(t0);
Texture2D distort_map : register(t1);
Texture2D background : register(t2);

SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD)
{
    PSInput result;

    result.position = mul(projViewMat, mul(modelMat, position));
    result.tcoord = tcoord;
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float2 distort = distort_map.Sample(g_sampler, input.tcoord).xy * 2.0 - 1.0;
    
    //float3 bgcolor = background.Sample(g_sampler, input.screen_pos + distort).rgb;
    float3 bgpos = input.position.xyz + float3(distort * 50, 0);
       
    bgpos.x = clamp(bgpos.x, 0, width);
    bgpos.y = clamp(bgpos.y, 0, height);
    
    float3 bgcolor = background.Load(int3(bgpos)).rgb;
    float4 color = pow(diffuse_map.Sample(g_sampler, input.tcoord), 2.2);
       
    return float4(lerp(color.xyz, bgcolor, 0.5), 1.0);
}
