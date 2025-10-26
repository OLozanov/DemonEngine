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
Texture2D depth : register(t1);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD)
{
    PSInput result;

    result.position = mul(projViewMat, position); //mul(projViewMat, mul(modelMat, position));
    result.tcoord = tcoord;
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    int3 screen_pos = int3(input.position.xy, 0);

    float3 color = pow(image.Sample(g_sampler, input.tcoord), 2.2);
    float z = depth.Load(screen_pos);
    
    if(z < 1.0) discard;
    
    return float4(color, 1.0);
}
