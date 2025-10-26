#include "material.hlsl"

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float padding[48];
};

cbuffer SceneConstantBuffer : register(b1)
{
    float4x4 modelMat;
};

cbuffer MaterialConstantBuffer : register(b2)
{
    float4 material_color;
    float material_metalness;
    float material_roughness;
    float material_luminosity;
    uint material_flags;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
    float alpha : ALPHA;
};

struct PSOutput
{
    float4 color: SV_Target0;
    float4 normal: SV_Target1;
    float4 flat_normal: SV_Target2;
    float4 params: SV_Target3;
};

Texture2D diffuse_map : register(t0);
Texture2D normal_map : register(t1);
Texture2D roughness_map : register(t2);
Texture2D metalness_map : register(t3);
Texture2D luminosity_map : register(t4);
Texture2D height_map : register(t5);

SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 tcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float3 binormal : BINORMAL, float alpha : ALPHA)
{
    PSInput result;

    result.position = mul(projViewMat, mul(modelMat, position));
    result.tcoord = tcoord;

    float3x3 normalMat = modelMat;
    
	result.normal = mul(normalMat, normal);
	result.tangent = mul(normalMat, tangent);
	result.binormal = mul(normalMat, binormal);
    
    result.alpha = alpha;

    return result;
}

PSOutput PSMain(PSInput input) : SV_TARGET
{
	float4 color = diffuse_map.Sample(g_sampler, input.tcoord);
    color.w *= input.alpha;
    
    if (color.w < 0.01) discard;
    
	float3 norm_local = normal_map.Sample(g_sampler, input.tcoord);
	norm_local.xyz = norm_local.xyz*2.0 - 1.0;
	
	float3 norm_world;
	norm_world.xyz = norm_local.x*input.tangent.xyz + 
					norm_local.y*input.binormal.xyz + 
					norm_local.z*input.normal.xyz;
	
	norm_world = normalize(norm_world);
				
	float4 out_norm;
	out_norm.xyz = norm_world.xyz*0.5 + 0.5;
	out_norm.w = color.w;
    
    float roughness; 
    float metalness; 
    float luminosity; 
    
    if (material_flags & RoughnessMap) 
        roughness = roughness_map.Sample(g_sampler, input.tcoord).r;
    else 
        roughness = material_roughness;
    
    if (material_flags & MetalnessMap)
        metalness = metalness_map.Sample(g_sampler, input.tcoord).r;
    else 
        metalness = material_metalness;
        
    if (material_flags & LuminosityMap) 
        luminosity = luminosity_map.Sample(g_sampler, input.tcoord).r;
    else
        luminosity = material_luminosity;
	
	PSOutput output;
	output.color = float4(color.xyz, color.w);
	output.normal = out_norm;
    output.flat_normal = float4(normalize(input.normal)*0.5 + 0.5, 1.0);
    output.params = float4(metalness, roughness, luminosity, color.w);
	
	return output;
}
