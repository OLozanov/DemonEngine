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
    uint matid;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

struct PSOutput
{
    float4 color: SV_Target0;
    float4 normal: SV_Target1;
    float4 flat_normal: SV_Target2;
    float4 params: SV_Target3;
};

StructuredBuffer<Material> materials : register(t0);
Texture2D image[] : register(t1);

SamplerState g_sampler : register(s0);

PSInput VSMain(float3 position : POSITION, float4 tcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float3 binormal : BINORMAL, float4 instpos : INSTANCEPOS)
{
    PSInput result;

	float3 scaledpos = float3(position.x, position.y * instpos.w, position.z);
    float4 pos = float4(scaledpos + instpos.xyz, 1.0);
    
    result.position = mul(projViewMat, mul(modelMat, pos));
    result.tcoord = tcoord;
    
    float3x3 normalMat = modelMat;
    
	result.normal = mul(normalMat, normal);
	result.tangent = mul(normalMat, tangent);
	result.binormal = mul(normalMat, binormal);

    return result;
}

PSOutput PSMain(PSInput input) : SV_TARGET
{
	if (input.position.w > 60.0) discard;
	
	float4 color = float4(materials[matid].color.xyz, 1.0);
	
	if (materials[matid].diffuse_map != InvalidImage)
		color *= image[materials[matid].diffuse_map].Sample(g_sampler, input.tcoord);
	
    if (color.w < 0.2) discard;
    
	float3 norm_local;
	
	if (materials[matid].normal_map != InvalidImage)
	{
		norm_local = image[materials[matid].normal_map].Sample(g_sampler, input.tcoord);
		norm_local.xyz = norm_local.xyz*2.0 - 1.0;
	}
	else
		norm_local = float3(0.0, 0.0, 1.0);
	
	float3 norm_world;
	norm_world.xyz = norm_local.x*input.tangent.xyz + 
				     norm_local.y*input.binormal.xyz + 
				     norm_local.z*input.normal.xyz;
	
	norm_world = normalize(norm_world);
    			
	float4 out_norm;
	out_norm.xyz = norm_world.xyz*0.5 + 0.5;
	out_norm.w = 1.0;
	
    float roughness; 
    float metalness; 
    float luminosity; 
    
    if (materials[matid].roughness_map != InvalidImage) 
        roughness = image[materials[matid].roughness_map].Sample(g_sampler, input.tcoord).r;
    else 
        roughness = materials[matid].roughness;
    
    if (materials[matid].metalness_map != InvalidImage)
        metalness = image[materials[matid].metalness_map].Sample(g_sampler, input.tcoord).r;
    else 
        metalness = materials[matid].metalness;
        
    if (materials[matid].luminosity_map != InvalidImage) 
        luminosity = image[materials[matid].luminosity_map].Sample(g_sampler, input.tcoord).r;
    else
        luminosity = materials[matid].luminosity;
	
	PSOutput output;
	output.color = float4(color.xyz, 1.0);
	output.normal = out_norm;
    output.flat_normal = float4(normalize(input.normal)*0.5 + 0.5, 1.0);
    output.params = float4(metalness, roughness, luminosity, 1.0);
	
	return output;
}
