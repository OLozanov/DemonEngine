#include "ShadingModel.hlsl"

struct OmniLightData
{
    float3 pos;
    float radius;
    float3 flux;
    float falloff;
    uint shadow_idx;
};

static const float2 quad[4] = {{ -1.0f, 1.0f },
                               { 1.0f, 1.0f },
                               { -1.0f, -1.0f },
                               { 1.0f, -1.0f }};
                               
cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 eyepos;
    float fovx;
    float fovy;
    float padding[27];
};

cbuffer LightData : register(b1)
{   
    uint id;
};

SamplerState g_sampler : register(s0);
SamplerComparisonState  g_cmpSampler : register(s1);

StructuredBuffer<OmniLightData> lights : register(t0);
Texture2D diffuse : register(t1);
Texture2D normal : register(t2);
Texture2D parameters : register(t3);
Texture2D depth : register(t4);
TextureCube shadow[] : register(t5);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 clip_pos : CLIP_POS;
};

PSInput VSMain(uint vid: SV_VertexID)
{
    PSInput result;
    
    float3 clip_pos;
    
    float3 ltvec = lights[id].pos - eyepos;    
    float ltsize = lights[id].radius + lights[id].falloff;
    
    float4 position = float4(quad[vid].x, quad[vid].y, 1.0, 1.0);
    
    if(dot(ltvec, ltvec) < ltsize*ltsize + 0.25)
    {
        result.position = position;
        clip_pos = float3(position.x*fovx, position.y*fovy, 1.0);
    }
    else
    {
        float4 pos = mul(projViewMat, float4(lights[id].pos, 1.0));
        float zpos = pos.w;
        pos.w = max(0.1, abs(pos.w));
        
        float screen_size = (lights[id].radius + lights[id].falloff + 0.5)/pos.w*fovx;
        float2 screen_pos = float2(position.x/fovx, position.y/fovy)*screen_size + pos.xy/pos.w;
        
        result.position = float4(screen_pos, zpos > -ltsize ? 1.0 : -1.0, 1.0);
    
        clip_pos = float3(screen_pos.x*fovx, screen_pos.y*fovy, 1.0);
    }
       
    result.clip_pos = mul(worldMat, clip_pos);
    
    return result;
}

float ExtractZCoord(float d)
{
    const float far = 5000.0;
    const float near = 0.1;
    
    const float A = far/(far - near);
    const float B = -far*near/(far - near);
    
    return B/(d - A);
    //return near * far / (far - d * (far - near));
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

float4 PSMain(PSInput input) : SV_TARGET
{
    int3 screen_pos = int3(input.position.xy, 0);
    
    float3 diffuse_color = pow(diffuse.Load(screen_pos).rgb, 2.2);
    float4 params = parameters.Load(screen_pos);
    
    float3 norm = 2.0*normal.Load(screen_pos) - 1.0;
    norm = normalize(norm);
    
    float z = ExtractZCoord(depth.Load(screen_pos));
    
    float3 clip_vec = input.clip_pos*z;
    float3 world_pos = clip_vec + eyepos; 
    
    float3 lvec = lights[id].pos - world_pos;
	float3 ldir = normalize(lvec);
	float dist = length(lvec);
       
    float ltCos = max(0.0, dot(ldir, norm));
    
    //specular
	float3 eyeVec = normalize(-clip_vec);
    
    // sphere attenuation
    float att = (dist < lights[id].radius) ? 1.0 :
                (dist < lights[id].radius + lights[id].falloff) ? 1.0 - (dist - lights[id].radius)/lights[id].falloff : 0.0;
    
    //shadow    
    float shad = 1.0;
	
    if (lights[id].shadow_idx != -1)
    {
        const int kernel_sz = 5;
        const int kernel_len = kernel_sz*2 + 1;
        const int probe_num = kernel_len*kernel_len;
        
        const float shadowRes = 1.0/1024;
        
        float3 u;
        float3 v;
        
        float lx = abs(lvec.x);
        float ly = abs(lvec.y);
        float lz = abs(lvec.z);
            
        if(abs(lx) > abs(ly) && abs(lx) > abs(lz))
        {
            u = float3(0, 0, 1)*lx;
            v = float3(0, 1, 0)*lx;
        }
        
        if(abs(ly) > abs(lx) && abs(ly) > abs(lz))
        {
            u = float3(1, 0, 0)*ly;
            v = float3(0, 0, 1)*ly;
        }
        
        if(abs(lz) > abs(lx) && abs(lz) > abs(ly))
        {
            u = float3(1, 0, 0)*lz;
            v = float3(0, 1, 0)*lz;
        }
        
        u *= shadowRes;
        v *= shadowRes;
    
        shad = 0.0;

        float d = DirToDepth(-lvec);
        //float shad = shadow[light_shadow_idx].SampleCmp(g_cmpSampler, -ldir, d);
        
        uint shadow_idx = lights[id].shadow_idx;
        
        [unroll]
        for(int i = -kernel_sz; i <= kernel_sz; i++)
        {
            for(int k = -kernel_sz; k <= kernel_sz; k++)
            {
                float3 probe = -lvec + i*u + k*v;			
                shad += shadow[shadow_idx].SampleCmp(g_cmpSampler, probe, d);
            }
        }
        
        shad /= probe_num;
    }
       
    float3 radiance = lights[id].flux*shad*att*4;
    float3 color = BRDF(norm, ldir, eyeVec, diffuse_color, radiance, params[1], params[0]);
    
    //color = color/(color + 1.0);
    //color = pow(color, 1.0/2.2);    
    
    return float4(color, 1.0);
}
