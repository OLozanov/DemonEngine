#include "material.hlsl"
#include "Light.hlsl"
#include "ShadingModel.hlsl"

#define blocksize 16
#define maxlights 128

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float padding[48];
};

cbuffer LightingConstantBuffer : register(b1)
{
    float3 topleft;
    float3 xdir;
    float3 ydir;
    float3 eyepos;
    
    uint xtiles;

    float3 light_dir;
    bool enable_dir_light;
    float3 light_flux;
    float3 ambient_color;
    bool ambient_buffer;
};

cbuffer ShadowConstantBuffer : register(b2)
{
    float4x4 shadowMat[3];
};

cbuffer ObjectConstantBuffer : register(b3)
{
    float4x4 modelMat;
};

cbuffer ScreenConstantBuffer : register(b4)
{
    uint width;
    uint height;
};

cbuffer MaterialConstantBuffer : register(b5)
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
	float3 worldpos : WORLDPOS;
    float2 tcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
};

StructuredBuffer<uint> light_grid : register(t0);
StructuredBuffer<OmniLightData> omni_lights : register(t1);
StructuredBuffer<SpotLightData> spot_lights : register(t2);

Texture2D diffuse_map : register(t3);
Texture2D normal_map : register(t4);
Texture2D roughness_map : register(t5);
Texture2D background : register(t6);

Texture2DArray shadow : register(t7);
TextureCube omni_shadow[] : register(t8);
Texture2D spot_shadow[] : register(t0, space1);

SamplerState g_sampler : register(s0);
SamplerComparisonState g_cmpSampler : register(s1);

#include "LightShadows.hlsl"

PSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float3 binormal : BINORMAL)
{
    PSInput result;

	float4 pos = mul(modelMat, position);

    result.position = mul(projViewMat, pos);
    result.tcoord = tcoord;
	
	float3x3 normalMat = modelMat;
    
	float3 norm = mul(normalMat, normal);
	float3 tang = mul(normalMat, tangent);
	float3 binorm = mul(normalMat, binormal);

    result.normal = norm;
    result.tangent = tang;
    result.binormal = binorm;
	result.worldpos = pos.xyz;
    
    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
	float3 eyevec = normalize(eyepos - input.worldpos);
	
	float3 diffuse = material_color.xyz;
	
	if (material_flags & DiffuseMap)
		diffuse *= pow(diffuse_map.Sample(g_sampler, input.tcoord).xyz, 2.2);
	
    //if (material_color.w < 0.2) discard;
    
	float3 norm_local;
	
	if (material_flags & NormalMap)
		norm_local = normal_map.Sample(g_sampler, input.tcoord).xyz * 2.0 - 1.0;
	else
		norm_local = float3(0.0, 0.0, 1.0);
	
	float3 norm_world = norm_local.x*input.tangent.xyz + 
				        norm_local.y*input.binormal.xyz + 
				        norm_local.z*input.normal.xyz;
	
	norm_world = normalize(norm_world);
    
    float3 bgpos = input.position.xyz + float3(norm_local.xy * 50, 0);
       
    bgpos.x = clamp(bgpos.x, 0, width);
    bgpos.y = clamp(bgpos.y, 0, height);
    
    float3 bgcolor = background.Load(int3(bgpos)).rgb;	
	float3 color = lerp(bgcolor, diffuse, material_color.w);
	
	float roughness; 
    
    if (material_flags & RoughnessMap) 
        roughness = roughness_map.Sample(g_sampler, input.tcoord).r;
    else 
        roughness = material_roughness;
	
	if (roughness == 1.0) return float4(color, 1.0);
	
	// directed global light
	if (enable_dir_light)
    {
        float shad = DirectedShadow(input.worldpos, norm_world);
        float3 radiance = light_flux*shad;
        color += BRDFSpec(norm_world, light_dir, eyevec, radiance, roughness);
    }
	
	uint2 tile_id = uint2(input.position.xy / blocksize);
	uint grid_offset = (tile_id.y * xtiles + tile_id.x) * maxlights;
    uint onum = light_grid[grid_offset] & 0xFF;
    uint snum = (light_grid[grid_offset] >> 8) & 0xFF;
	
	// omni lights
	for (uint i = 1; i <= onum; i++)
    {
        uint id = light_grid[grid_offset + i];
        
        float3 lvec = omni_lights[id].pos - input.worldpos;
        float3 ldir = normalize(lvec);
        float dist = length(lvec);
                     
        // sphere attenuation
        float att = (dist < omni_lights[id].radius) ? 1.0 :
                    (dist < omni_lights[id].radius + omni_lights[id].falloff) ? 1.0 - (dist - omni_lights[id].radius)/omni_lights[id].falloff : 0.0;
        
        //shadow
        uint shadow_idx = omni_lights[id].shadow_idx;

        float shad;
        if (shadow_idx != -1) 
            shad = OmniShadow(shadow_idx, lvec);
        else
            shad = 1.0;
        
        float3 radiance = omni_lights[id].flux*shad*att*4;
        color += BRDFSpec(norm_world, ldir, eyevec, radiance, roughness);
    }
	
	// spot lights
	for (uint i = 1; i <= snum; i++)
    {
        uint id = light_grid[grid_offset + onum + i];
    
        float3 lvec = spot_lights[id].pos - input.worldpos;
        float3 ldir = normalize(lvec);
        float dist = length(lvec);
                          
        // sphere attenuation
        float att = (dist < spot_lights[id].radius) ? 1.0 :
                    (dist < spot_lights[id].radius + spot_lights[id].falloff) ? 1.0 - (dist - spot_lights[id].radius)/spot_lights[id].falloff : 0.0;
        
        // cone attenuation
        float ang = dot(-ldir, spot_lights[id].dir);
        att *= (ang < spot_lights[id].angle_outer) ? 0.0 :
               (ang < spot_lights[id].angle_inner) ? (ang - spot_lights[id].angle_outer) / (spot_lights[id].angle_inner - spot_lights[id].angle_outer) : 1.0;
       
        uint shadow_idx = spot_lights[id].shadow_idx;
        
        float shad;
        if (shadow_idx != -1) 
            shad = SpotShadow(id, input.worldpos);
        else 
            shad = 1.0;
        
        float3 radiance = spot_lights[id].flux*att*shad*4;
        color += BRDFSpec(norm_world, ldir, eyevec, radiance, roughness);
    }
   
	return float4(color, 1.0);
}
