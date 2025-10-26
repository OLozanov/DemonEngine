cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 shadowMat;
};

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelMat;
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

    result.position = mul(shadowMat, mul(modelMat, position));
    result.tcoord = tcoord;
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{  
    return float4(1.0, 0.0, 0.0, 1.0);
}
