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
    float3 rangeColor;
    float radius;
    float3 center;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float dist : DIST;
};

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;
    
    float4 pos = mul(modelMat, position);

    result.position = mul(projViewMat, pos);
    result.dist = length(pos.xyz - center);

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    if (input.dist < radius) 
    {   
        float factor = min((radius - input.dist) * 4, 1.0);
        float3 color = lerp(colorAlpha.xyz, rangeColor, factor);
    
        return float4(color, colorAlpha.w);
    }
    else return colorAlpha;
}
