#include "material.hlsl"

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

StructuredBuffer<Material> materials : register(t0);
StructuredBuffer<uint> faces : register(t1);

Texture2D image[] : register(t2);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD)
{
    PSInput result;

    result.position = mul(projViewMat, mul(modelMat, position));
    result.tcoord = tcoord;
    
    return result;
}

float4 PSMain(PSInput input, uint face : SV_PrimitiveID) : SV_TARGET
{
	uint matid = faces[face];
	uint diffuse = materials[matid].diffuse_map;
    float4 color = colorAlpha;

	if (diffuse != InvalidImage) color *= image[diffuse].Sample(g_sampler, input.tcoord);
    
    if (color.w < 0.01) discard; 
	
    return float4(color.xyz, 1.0);
}
