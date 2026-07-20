#include "material.hlsl"

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 eyepos;
	float fovx;
	float fovy;
	float3 pad;
	float3 topleft;
    float3 xdir;
    float3 ydir;
};

cbuffer DecalConstantBuffer : register(b1)
{
	float3x4 decalMat;
	float3 size;
    uint matid;
};

struct PSInput
{
    float4 position : SV_POSITION;
};

struct PSOutput
{
    float4 color: SV_Target0;
    float4 normal: SV_Target1;
    float4 flat_normal: SV_Target2;
    float4 params: SV_Target3;
};

StructuredBuffer<Material> materials : register(t0);
Texture2D depth : register(t1);
Texture2D image[] : register(t2);

SamplerState g_sampler : register(s0);

PSInput VSMain(float3 position : POSITION)
{
    PSInput result;

    float3 world_pos = mul(decalMat, float4(position * size, 1.0));

    result.position = mul(projViewMat, float4(world_pos, 1.0));   
    return result;
}

float ExtractZCoord(float d)
{
    const float far = 5000.0;
    const float near = 0.1;
    
    const float A = far/(far - near);
    const float B = -far*near/(far - near);
    
    return B /(d - A);
}

PSOutput PSMain(PSInput input) : SV_TARGET
{
	float d = depth.Load(int3(input.position.xy, 0)).r;
    if (d == 1.0) discard;
	
	float z = ExtractZCoord(d);
	
	float dist = max(size.x, size.y) * 500.0;
	if (z > dist) discard;
    
    float3 view_vec = topleft + xdir * (input.position.x + 0.5) + ydir * (input.position.y + 0.5);
    float3 fragpos = view_vec * z + eyepos - decalMat._m03_m13_m23;
	
	float3 rsz = 1.0 / size;
	
	float u = dot(decalMat._m00_m10_m20, fragpos) * rsz.x;
	float v = dot(decalMat._m01_m11_m21, fragpos) * rsz.y;
	float w = dot(decalMat._m02_m12_m22, fragpos) * rsz.z;

	if (abs(u) > 1.0) discard;
	if (abs(v) > 1.0) discard;
	if (abs(w) > 1.0) discard;
	
	float2 tcoord = float2(u, v) * 0.5 + 0.5;
	
	float4 color = image[materials[matid].diffuse_map].Sample(g_sampler, tcoord);
	
	if (color.w < 0.1) discard;
	
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
	
	float3 norm_world;
	
	if (materials[matid].normal_map != InvalidImage)
	{
		float3 norm_local = image[materials[matid].normal_map].Sample(g_sampler, tcoord);
		norm_local.xyz = norm_local.xyz * 2.0 - 1.0;
		
		norm_world.xyz = norm_local.x * decalMat._m00_m10_m20 + 
						 norm_local.y * decalMat._m01_m11_m21 + 
						 norm_local.z * decalMat._m02_m12_m22;
						 
		norm_world = normalize(norm_world);
	}
	else
		norm_world = decalMat._m02_m12_m22;
	
	PSOutput output;
	output.color = float4(color.xyz, 1.0);
	output.normal = float4(norm_world.xyz * 0.5 + 0.5, 1.0);
	output.params = float4(metalness, roughness, luminosity, 1.0);
	
	return output;
}