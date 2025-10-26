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
};

Texture2D image : register(t0);
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
    float4 texel = image.Sample(g_sampler, input.tcoord);
    
    if (texel.w < 0.01)
        discard;
    
    float3 color = pow(texel.xyz * colorAlpha.xyz, 2.2);
    float alpha = texel.w * colorAlpha.w;
    
    return float4(color, alpha);
}
