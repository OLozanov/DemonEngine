#include "material.hlsl"

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 eyepos;
	float padding[29];
};

cbuffer ObjectConstantBuffer : register(b1)
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
    float3 viewdir : VIEWDIR;
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

PSInput VSMain(float4 position : POSITION, float4 tcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float3 binormal : BINORMAL)
{
    PSInput result;

    float4 world_pos = mul(modelMat, position);
    
    result.position = mul(projViewMat, world_pos);
    result.tcoord = tcoord;

    float3x3 normalMat = modelMat;
    
	float3 norm = mul(normalMat, normal);
	float3 tang = mul(normalMat, tangent);
	float3 binorm = mul(normalMat, binormal);
    
    float3 viewdir = world_pos.xyz - eyepos;

    result.normal = norm;
    result.tangent = tang;
    result.binormal = binorm;
    
    result.viewdir = float3(dot(tang, viewdir),
                            -dot(binorm, viewdir),
                            dot(norm, viewdir));
    
    return result;
}

float2 Paralax(float2 tcoord, float3 viewdir)
{
    const uint layers = 20;
    const float layer_depth = 1.0 / layers;

    float2 p = viewdir.xy / viewdir.z * 0.05;
    float2 step = p * layer_depth;

    float2 trace_coord = tcoord;
	
	Texture2D height_map = image[materials[matid].height_map]; 
    
    float lod = height_map.CalculateLevelOfDetail(g_sampler, tcoord);
    
    float trace_depth = 0.0;    
    float depth = 1.0 - height_map.Sample(g_sampler, tcoord).r;
    float prev_depth = depth;
    
    uint i = 0;
    for (; i <= layers; i++)
    {   
        if (trace_depth > depth) break;
            
        trace_coord -= step;
        prev_depth = depth;
        depth = 1.0 - height_map.SampleLevel(g_sampler, trace_coord, lod).r;
        
        trace_depth += layer_depth;
    }
    
    if (i > 0 && i < layers)
    {
        float2 prev_coord = trace_coord + step;
        
        float adepth = depth - trace_depth;
        float bdepth = prev_depth - (trace_depth - layer_depth);
        
        float w = adepth / (adepth - bdepth);
        
        trace_coord = prev_coord * w + trace_coord * (1.0 - w);    
    }
    
    return trace_coord;
}

PSOutput PSMain(PSInput input) : SV_TARGET
{
    float2 tcoord;
    
    if (materials[matid].height_map != InvalidImage)
        tcoord = Paralax(input.tcoord, input.viewdir);
    else 
        tcoord = input.tcoord;
    
	float4 color = float4(materials[matid].color.xyz, 1.0);
	
	if (materials[matid].diffuse_map != InvalidImage)
		color *= image[materials[matid].diffuse_map].Sample(g_sampler, tcoord);
	
    if (color.w < 0.2) discard;
    
	float3 norm_local;
	
	if (materials[matid].normal_map != InvalidImage)
	{
		norm_local = image[materials[matid].normal_map].Sample(g_sampler, tcoord);
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
        roughness = image[materials[matid].roughness_map].Sample(g_sampler, tcoord).r;
    else 
        roughness = materials[matid].roughness;
    
    if (materials[matid].metalness_map != InvalidImage)
        metalness = image[materials[matid].metalness_map].Sample(g_sampler, tcoord).r;
    else 
        metalness = materials[matid].metalness;
        
    if (materials[matid].luminosity_map != InvalidImage) 
        luminosity = image[materials[matid].luminosity_map].Sample(g_sampler, tcoord).r;
    else
        luminosity = materials[matid].luminosity;
    
	PSOutput output;
	output.color = float4(color.xyz, 1.0);
	output.normal = out_norm;
    output.flat_normal = float4(normalize(input.normal)*0.5 + 0.5, 1.0);
    output.params = float4(metalness, roughness, luminosity, 1.0);
	
	return output;
}
