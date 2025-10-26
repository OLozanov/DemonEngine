#include "ShadingModel.hlsl"

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
    float3 light_dir;
    float pad1;
    float3 light_flux;
    float pad2;
};

cbuffer ShadowData : register(b2)
{
    float4x4 shadowMat[3];
};

SamplerState g_sampler : register(s0);
SamplerComparisonState  g_cmpSampler : register(s1);

Texture2D diffuse : register(t0);
Texture2D normal : register(t1);
Texture2D parameters : register(t2);
Texture2D depth : register(t3);
Texture2DArray shadow : register(t4);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 clip_pos : CLIP_POS;
};

PSInput VSMain(uint vid: SV_VertexID)
{
    PSInput result;
    
    float3 clip_pos = float3(quad[vid].x*fovx, quad[vid].y*fovy, 1.0);
   
    result.position = float4(quad[vid].x, quad[vid].y, 1.0, 1.0);      
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
    
	float3 eyeVec = normalize(-clip_vec);
          
    //shadow, make sense to move this to separate func
    float2 ltpos = mul(shadowMat[0], float4(world_pos, 1.0)).xy;
    
    int shadowLod = 0;
	float blurScale = 1;
	
	if (abs(ltpos.x) > 0.95 || abs(ltpos.y) > 0.95)
	{
		shadowLod = 1;
		blurScale = 0.33;
	}
    else  if (abs(ltpos.x) > 2.95 || abs(ltpos.y) > 2.95)
    {
    	shadowLod = 2;
		blurScale = 0.33 * 0.33;
    }
    
    float zbias = max(0.0001 * (1.0 - abs(dot(norm, light_dir))), 0.0001); 
    
    float4 shadPos = mul(shadowMat[shadowLod], float4(world_pos, 1.0));
    shadPos.xy = shadPos.xy*0.5 + 0.5;
    shadPos.y = -shadPos.y;
    shadPos.z -= zbias; //0.0001;
    //float shad = shadow.SampleCmp(g_cmpSampler, shadPos.xy, shadPos.z);
    
    const int kernel_sz = 3;
	const int kernel_len = kernel_sz*2 + 1;
	const int probe_num = kernel_len*kernel_len;
    
    const float shadowRes = 1.0/2048;
    
    float shad = 0;
	
    [unroll]
	for(int i = -kernel_sz; i <= kernel_sz; i++)
	{
        [unroll]
		for(int k = -kernel_sz; k <= kernel_sz; k++)
		{
			float2 probe = shadPos.xy + float2(i, k)*shadowRes*blurScale;
			shad += shadow.SampleCmp(g_cmpSampler, float3(probe.x, probe.y, shadowLod), shadPos.z);
		}
	}
	
	shad /= probe_num;
    
    //
    float3 radiance = light_flux*shad;
    float3 color = BRDF(norm, light_dir, eyeVec, diffuse_color, radiance, params[1], params[0]);
    
    //float3 n = normal.Load(screen_pos);
    
    return float4(color, 1.0);
}