#include "Light.hlsl"

#define blocksize 16
#define maxlights 128

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 viewpos;
    float fovx;
    float fovy;
    //float3 pad;
    //float3 topleft;
    //float3 xdir;
    //float3 ydir;
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

cbuffer ParamsConstantBuffer : register(b2)
{
    float3 pos;
    float3 size;
    float density;
    float3 fogColor;
    uint flags;
};

struct PSInput
{
    float4 position : SV_POSITION;
};

SamplerComparisonState g_cmpSampler : register(s0);

Texture2D depth : register(t0);

StructuredBuffer<uint> light_grid : register(t1);
StructuredBuffer<OmniLightData> omni_lights : register(t2);
StructuredBuffer<SpotLightData> spot_lights : register(t3);

Texture2DArray dir_shadow : register(t4);
TextureCube omni_shadow[] : register(t5);
Texture2D spot_shadow[] : register(t0, space1);

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;

    float4 world_pos = float4(position.xyz * size + pos, 1.0);
    result.position = mul(projViewMat, world_pos);

    return result;
}

// Random number in [0, 1]
float random(float2 v)
{
    return frac(sin(dot(v, float2(41, 289)))*45758.5453);
}

float ExtractZCoord(float d)
{
    const float far = 5000.0;
    const float near = 0.1;
    
    const float A = far/(far - near);
    const float B = -far*near/(far - near);
    
    return B/(d - A);
}

float DirToDepth(float3 dir)
{
    float3 adir = abs(dir);
    float z = max(adir.x, max(adir.y, adir.z));
	z -= 0.1;

    const float far = 500.0;
    const float near = 0.1;
    
    const float A = far/(far - near);
    const float B = -far*near/(far - near);
    
    return B/z + A;
}

bool TraceBox(float3 origin, float3 ray, out float tmin, out float tmax)
{
    tmin = 0;
    tmax = 1.#INF;

    for (uint i = 0; i < 3; i++)
    {
        if (abs(ray[i]) < 0.0001)
        {
            if ((origin[i] < -size[i]) || (origin[i] > size[i])) return false;
            else continue;
        }

        float dst1 = (origin[i] - size[i]) / -ray[i];
        float dst2 = (origin[i] + size[i]) / -ray[i];

        float dmin;
        float dmax;

        if (dst1 < dst2)
        {
            dmin = dst1;
            dmax = dst2;
        }
        else
        {
            dmin = dst2;
            dmax = dst1;
        }

        if (dmax < 0) return false;
        if (dmin < 0) dmin = 0;

        if (dmin > tmin) tmin = dmin;
        if (dmax < tmax) tmax = dmax;

        if (tmin > tmax) return false;
    }

    tmin = max(0.1, tmin);
    tmax = max(0.1, tmax);
    
    //tinfo.dist = tmin;
    
    return true;
}

bool TraceSphere(float3 origin, float3 ray, out float tmin, out float tmax, out float dens)
{
    float rad = size[0];
    float3 dir = pos - origin;
    
    float len2 = dot(dir, dir);
    float rad2 = rad * rad;

    //if(len2 < rad2) return false;

    float rayproj = dot(dir, ray);
    float rdist2 = len2 - rayproj * rayproj;

    if(rdist2 > rad2) return false;
    
    float idist = sqrt(rad2 - rdist2);

    tmin = max(0.1, rayproj - idist);
    tmax = max(0.1, rayproj + idist);

    float rdist = sqrt(rdist2);
    dens = min(0.5, 1.0 - rdist / rad) * 2.0;
    
    return true;
}

float OmniShadow(uint shadow_idx, float3 lvec)
{   
    float d = DirToDepth(-lvec);
    
    return omni_shadow[shadow_idx].SampleCmpLevelZero(g_cmpSampler, -lvec, d);
}

float SpotShadow(uint id, float3 world_pos)
{
    uint shadow_idx = spot_lights[id].shadow_idx;
    
    float4 shadPos = mul(spot_lights[id].shadow_mat, float4(world_pos, 1.0));
    shadPos.xyz /= shadPos.w;
    shadPos.xy = shadPos.xy*0.5 + 0.5;
    shadPos.y = -shadPos.y;
    //shadPos.z -= 0.0001;
          
    return spot_shadow[shadow_idx].SampleCmpLevelZero(g_cmpSampler, shadPos.xy, shadPos.z);
}

float3 TraceLight(int2 screen_pos, float3 origin, float3 ray, float tmin, float tmax)
{
    const uint steps = 64;
    const float rsteps = 1.0 / steps;

    int2 tile = screen_pos / blocksize;
    
    uint grid_offset = (tile.y * xtiles + tile.x) * maxlights;
    uint onum = light_grid[grid_offset] & 0xFF;
    uint snum = (light_grid[grid_offset] >> 8) & 0xFF;
    
    float3 color = 0.0;
    
    float offset = random(screen_pos);
    
    for (uint k = 0; k < steps; k++)
    {
        float t = tmin + (tmax - tmin) * rsteps * (k + offset);
        float3 pos = origin + ray * t;
        
        for (uint i = 1; i <= onum; i++)
        {
            uint id = light_grid[grid_offset + i];
            
            float3 lvec = omni_lights[id].pos - pos;
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
            
            float3 radiance = omni_lights[id].flux*att*shad;
            color += radiance;
        }
        
        for (uint i = 1; i <= snum; i++)
        {
            uint id = light_grid[grid_offset + onum + i];
        
            float3 lvec = spot_lights[id].pos - pos;
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
                shad = SpotShadow(id, pos);
            else
                shad = 1.0;
            
            float3 radiance = spot_lights[id].flux*att*shad;
            color += radiance;
        }
    }
    
    return color * rsteps + float3(0.5, 0.5, 0.5);
}

float4 PSMain(PSInput input) : SV_TARGET
{
    const uint type = flags & 1;
    const uint lighting = (flags >> 1) & 1;
    
    int3 screen_pos = int3(input.position.xy, 0);
    float z = ExtractZCoord(depth.Load(screen_pos).r);
    
    float3 view_vec = topleft + xdir * (screen_pos.x + 0.5) + ydir * (screen_pos.y + 0.5);
    
    float dfactor = 1.0;
    float tmin, tmax;
    float rlen = 1.0/length(view_vec);
    
    if (type == 0)
    {
        if(!TraceBox(eyepos - pos, view_vec * rlen, tmin, tmax))
            discard;
    }
    else
    {   
        if(!TraceSphere(eyepos, view_vec * rlen, tmin, tmax, dfactor))
            discard;
    }
    
    //return float4(tmax * 0.1, 0.0, 0.0, 1.0);
    
    tmin *= rlen;
    tmax *= rlen;
    
    tmin = min(tmin, z);
    tmax = min(tmax, z);
    
    float3 color;
    
    if (lighting == 1) 
        color = TraceLight(screen_pos.xy, eyepos, view_vec, tmin, tmax);
    else
        color = float3(1.0, 1.0, 1.0);
        
    float dist = abs(tmax - tmin);
    float factor = dist * density * dfactor;
    
    float fogFactor = exp(-factor);
           
    return float4(color * fogColor, 1.0-fogFactor);
}
