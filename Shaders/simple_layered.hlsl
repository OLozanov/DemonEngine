cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float padding[48];
};

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelMat;
};

cbuffer ParamsConstantBuffer : register(b2)
{
    float4 colorAlpha;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
    float alpha : ALPHA;
};

Texture2D image : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD, float alpha : ALPHA)
{
    PSInput result;

    result.position = mul(projViewMat, mul(modelMat, position));
    result.tcoord = tcoord;
    result.alpha = alpha;
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 color = image.Sample(g_sampler, input.tcoord); //pow(image.Sample(g_sampler, input.tcoord), 2.2);  
    color.w *= input.alpha;
    
    if (color.w < 0.01) discard;
    
    return float4(color.xyz, color.w);
}
