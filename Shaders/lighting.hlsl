#include "Light.hlsl"
#include "ShadingModel.hlsl"

#define blocksize 16
#define maxlights 128

cbuffer LightingConstantBuffer : register(b0)
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

cbuffer ShadowConstantBuffer : register(b1)
{
    float4x4 shadowMat[3];
};

SamplerComparisonState g_cmpSampler : register(s0);

RWTexture2D<half4> renderTarget : register(u0);

StructuredBuffer<uint> light_grid : register(t0);
StructuredBuffer<OmniLightData> omni_lights : register(t1);
StructuredBuffer<SpotLightData> spot_lights : register(t2);

Texture2D diffuse : register(t3);
Texture2D normal : register(t4);
Texture2D parameters : register(t5);
Texture2D depth : register(t6);

Texture2D ambient : register(t7);

Texture2DArray shadow : register(t8);
TextureCube omni_shadow[] : register(t9);
Texture2D spot_shadow[] : register(t0, space1);

float ExtractZCoord(float d)
{
    const float far = 5000.0;
    const float near = 0.1;
    
    const float A = far/(far - near);
    const float B = -far*near/(far - near);
    
    return B/(d - A);
}

#include "LightShadows.hlsl"

[numthreads(blocksize, blocksize, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    float d = depth.Load(DTid);
    if(d == 1.0) return;
    
    float3 clip_pos = topleft + xdir * (DTid.x + 0.5) + ydir * (DTid.y + 0.5);
    
    float3 diffuse_color = pow(diffuse.Load(DTid).rgb, 2.2);
    float4 params = parameters.Load(DTid);
    
    float3 norm = 2.0*normal.Load(DTid) - 1.0;
    norm = normalize(norm);
    
    float z = ExtractZCoord(d);
    
    float3 clip_vec = clip_pos * z;
    float3 world_pos = clip_vec + eyepos;
    
	float3 eyeVec = normalize(-clip_vec);
    
    //
    float3 ambcolor = ambient_buffer ? ambient[DTid.xy] + 0.005 : ambient_color;
    float3 color = ambcolor * diffuse_color;
    
    if (enable_dir_light)
    {
        float shad = DirectedShadow(world_pos, norm);
        float3 radiance = light_flux*shad;
        color += BRDF(norm, light_dir, eyeVec, diffuse_color, radiance, params[1], params[0]);
    }
    
    uint grid_offset = (Gid.y * xtiles + Gid.x) * maxlights;
    uint onum = light_grid[grid_offset] & 0xFF;
    uint snum = (light_grid[grid_offset] >> 8) & 0xFF;
    
    //half val = onum * 0.1;
    //renderTarget[DTid.xy] = half4(color + val, 1.0);
    //return;
    
    for (uint i = 1; i <= onum; i++)
    {
        uint id = light_grid[grid_offset + i];
        
        float3 lvec = omni_lights[id].pos - world_pos;
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
        color += BRDF(norm, ldir, eyeVec, diffuse_color, radiance, params[1], params[0]);
    }
    
    for (uint i = 1; i <= snum; i++)
    {
        uint id = light_grid[grid_offset + onum + i];
    
        float3 lvec = spot_lights[id].pos - world_pos;
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
            shad = SpotShadow(id, world_pos);
        else 
            shad = 1.0;
        
        float3 radiance = spot_lights[id].flux*att*shad*4;
        color += BRDF(norm, ldir, eyeVec, diffuse_color, radiance, params[1], params[0]);
    }
    
    float luminance = params[2];    
    color = max(diffuse_color * luminance, color);
    
    renderTarget[DTid.xy] = half4(color, 1.0);
    
    //half val = (GTid.x == 0 || GTid.y == 0) ? 0.5 : 0.2;
    //renderTarget[DTid.xy] = half4(val, val, val, 1.0);
}