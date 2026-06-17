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

cbuffer LayersConstantBuffer : register(b2)
{
	uint baseMaterial;
	uint layernum;
	uint width;
	uint height;
	uint4 layers[4];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 binormal : BINORMAL;
	float2 maskcoord : MASKCOORD;
};

struct PSOutput
{
    float4 color: SV_Target0;
    float4 normal: SV_Target1;
    float4 flat_normal: SV_Target2;
    float4 params: SV_Target3;
};

StructuredBuffer<Material> materials : register(t0);
StructuredBuffer<float> layer_masks : register(t1);
Texture2D image[] : register(t2);

SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float4 tcoord : TEXCOORD, float3 normal : NORMAL, float3 tangent : TANGENT, float3 binormal : BINORMAL, uint vid: SV_VertexID)
{
    PSInput result;

    result.position = mul(projViewMat, mul(modelMat, position));
    result.tcoord = tcoord;

    float3x3 normalMat = modelMat;
    
	result.normal = mul(normalMat, normal);
	result.tangent = mul(normalMat, tangent);
	result.binormal = mul(normalMat, binormal);
	
	uint y = vid / width;
	uint x = vid % width;
	
	result.maskcoord.x = x;
	result.maskcoord.y = y;

    return result;
}

void MaterialParameters(uint matid, PSInput input, out float4 color, out float3 normal, out float3 params)
{
	color = float4(materials[matid].color.xyz, 1.0);
	
	if (materials[matid].diffuse_map != InvalidImage)
		color *= image[materials[matid].diffuse_map].Sample(g_sampler, input.tcoord);
	   
	float3 norm_local;
	
	if (materials[matid].normal_map != InvalidImage)
	{
		norm_local = image[materials[matid].normal_map].Sample(g_sampler, input.tcoord);
		norm_local.xyz = norm_local.xyz*2.0 - 1.0;
	}
	else
		norm_local = float3(0.0, 0.0, 1.0);
	
	normal = norm_local.x*input.tangent.xyz + 
		     norm_local.y*input.binormal.xyz + 
			 norm_local.z*input.normal.xyz;
	
	normal = normalize(normal);

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
		
	params = float3(metalness, roughness, luminosity);
}

PSOutput PSMain(PSInput input) : SV_TARGET
{
	if (layernum == 0)
	{
		float4 color;
		float3 normal;
		float3 params;
	
		MaterialParameters(baseMaterial, input, color, normal, params);
		
		PSOutput output;
		output.color = float4(color.xyz, 1.0);
		output.normal = float4(normal*0.5 + 0.5, 1.0);
		output.flat_normal = float4(normalize(input.normal)*0.5 + 0.5, 1.0);
		output.params = float4(params.xyz, 1.0);
		
		return output;
	}
	
	float3 color = float3(0, 0, 0);
	float3 normal = float3(0, 0, 0);
	float3 params = float3(0, 0, 0);
	float ralpha = 1.0;
	    
	uint x1 = input.maskcoord.x;
	uint y1 = input.maskcoord.y;
	
	uint x2 = min(x1 + 1, width);
	uint y2 = min(y1 + 1, height);
	
	float fx = input.maskcoord.x - x1;
	float fy = input.maskcoord.y - y1;
	
	float rfx = 1.0 - fx;
	float rfy = 1.0 - fy;
	
	float4 weights = float4(rfx * rfy, fx * rfy, rfx * fy, fx * fy);
		
	for (int i = layernum - 1; i >= 0; i--)
	{				
		uint baseptr = width * height * i;
		
		float4 values = float4(layer_masks[baseptr + y1 * width + x1],
		                       layer_masks[baseptr + y1 * width + x2],
		                       layer_masks[baseptr + y2 * width + x1],
		                       layer_masks[baseptr + y2 * width + x2]);
		
		float alpha = dot(values, weights);
					  
		if (WaveActiveAllTrue(alpha < 0.001)) continue;

		uint l = i * 2;
		uint l1 = l / 4;
		uint l2 = l % 4;
		uint material = layers[l1][l2];
		
		float4 layer_color;
		float3 layer_normal;
		float3 layer_params;
		
		MaterialParameters(material, input, layer_color, layer_normal, layer_params);
					
		color += layer_color.xyz * alpha * ralpha;
		normal += layer_normal * alpha * ralpha;
		params += layer_params * alpha * ralpha;
		ralpha *= 1.0 - alpha;
		
		if (WaveActiveAllTrue(alpha > 0.999)) 
		{
			normal = normalize(normal);

			PSOutput output;
			output.color = float4(color.xyz, 1.0);
			output.normal = float4(normal*0.5 + 0.5, 1.0);
			output.flat_normal = float4(normalize(input.normal)*0.5 + 0.5, 1.0);
			output.params = float4(params.xyz, 1.0);

			return output;
		}
	}
	
	float4 base_color;
	float3 base_normal;
	float3 base_params;
	
	MaterialParameters(baseMaterial, input, base_color, base_normal, base_params);
	
	color += base_color.xyz * ralpha;
	normal += base_normal * ralpha;
	params += base_params * ralpha;
	
	normal = normalize(normal);

	PSOutput output;
	output.color = float4(color.xyz, 1.0);
	output.normal = float4(normal*0.5 + 0.5, 1.0);
	output.flat_normal = float4(normalize(input.normal)*0.5 + 0.5, 1.0);
	output.params = float4(params.xyz, 1.0);
	
	return output;
}
