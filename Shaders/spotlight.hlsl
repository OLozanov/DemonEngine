#include "ShadingModel.hlsl"

struct SpotLightData
{
    float3 pos;
    float radius;
    float3 flux;
    float falloff;
    float3 dir;
    float angle_outer;
    float angle_inner;
    uint shadow_idx;
    float padding[2];
    float4x4 shadow_mat;
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

SamplerComparisonState  g_cmpSampler : register(s0);

StructuredBuffer<SpotLightData> lights : register(t0);
Texture2D diffuse : register(t1);
Texture2D normal : register(t2);
Texture2D parameters : register(t3);
Texture2D depth : register(t4);
Texture2D shadow[] : register(t5);

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
        
        float screen_size = (lights[id].radius + lights[id].falloff + 0.5)/pos.w*fovx;
        float2 screen_pos = float2(position.x/fovx, position.y/fovy)*screen_size + pos.xy/pos.w;
        
        result.position = float4(screen_pos, pos.w > -ltsize ? 1.0 : -1.0, 1.0);
    
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
         
    //specular
	float3 eyeVec = normalize(-clip_vec);
	      
    // sphere attenuation
    float att = (dist < lights[id].radius) ? 1.0 :
                (dist < lights[id].radius + lights[id].falloff) ? 1.0 - (dist - lights[id].radius)/lights[id].falloff : 0.0;
    
    // cone attenuation
    float ang = dot(-ldir, lights[id].dir);
    att *= (ang < lights[id].angle_outer) ? 0.0 :
           (ang < lights[id].angle_inner) ? (ang - lights[id].angle_outer) / (lights[id].angle_inner - lights[id].angle_outer) : 1.0;

    float shad = 1.0;
          
    if (lights[id].shadow_idx != -1)
    {
        uint shadow_idx = lights[id].shadow_idx;
    
        float4 shadPos = mul(lights[id].shadow_mat, float4(world_pos, 1.0));
        shadPos.xyz /= shadPos.w;
        shadPos.xy = shadPos.xy*0.5 + 0.5;
        shadPos.y = -shadPos.y;
        shadPos.z -= 0.0001;
        
        shad = 0.0;
        
        const int kernel_sz = 5;
        const int kernel_len = kernel_sz*2 + 1;
        const int probe_num = kernel_len*kernel_len;
        
        const float shadowRes = 1.0/1048;
        
        shad = 0;
        
        [unroll]
        for(int i = -kernel_sz; i <= kernel_sz; i++)
        {
            [unroll]
            for(int k = -kernel_sz; k <= kernel_sz; k++)
            {
                float2 probe = shadPos.xy + float2(i, k)*shadowRes;
                shad += shadow[shadow_idx].SampleCmp(g_cmpSampler, probe, shadPos.z);
            }
        }
        
        shad /= probe_num;
    }
       
    float3 radiance = lights[id].flux*att*shad*4;
    float3 color = BRDF(norm, ldir, eyeVec, diffuse_color, radiance, params[1], params[0]);
    
    return float4(color, 1.0);
}
