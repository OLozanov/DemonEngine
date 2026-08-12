#include "material.hlsl"

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float padding[48];
};

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelMat;
};

cbuffer ParamsConstantBuffer : register(b2)
{
    float4 colorAlpha;
};

cbuffer LayersConstantBuffer : register(b3)
{
	uint baseMaterial;
	uint baseLayer;			// Base layer type
	uint layernum;
	uint width;
	uint height;
};

struct PSInput
{
    float4 position : SV_POSITION;
	float3 worldpos : WORLDPOS;
    float2 tcoord : TEXCOORD;
	float3 normal : NORMAL;
	float2 maskcoord : MASKCOORD;
};

StructuredBuffer<Material> materials : register(t0);
StructuredBuffer<float> layer_masks : register(t1);
StructuredBuffer<uint> layers : register(t2);

Texture2D image[] : register(t3);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD, float3 norm: NORMAL, uint vid: SV_VertexID)
{
    PSInput result;

    result.position = mul(projViewMat, mul(modelMat, position));
	result.worldpos = position.xyz;
    result.tcoord = tcoord;
	result.normal = norm;
	
	uint y = vid / width;
	uint x = vid % width;
	
	result.maskcoord.x = x;
	result.maskcoord.y = y;
    
    return result;
}

float3 sampleTriplanar(uint id, float3 pos, float3 norm)
{
	float3 w = pow(abs(norm), 5);
	w *= 1.0 / (w.x + w.y + w.z);
	
	float3 x = image[id].Sample(g_sampler, pos.yz * 0.5).xyz;
	float3 y = image[id].Sample(g_sampler, pos.xz * 0.5).xyz;
	float3 z = image[id].Sample(g_sampler, pos.xy * 0.5).xyz;
	
	return (x * w.x + y * w.y + z * w.z);
}

float4 PSMain(PSInput input) : SV_TARGET
{
	if (layernum == 0)
	{
		uint diffuse = materials[baseMaterial].diffuse_map;
		
		float3 color;
		
		if (baseLayer == 0)
			color = image[diffuse].Sample(g_sampler, input.tcoord).xyz;
		else
			color = sampleTriplanar(diffuse, input.worldpos, normalize(input.normal));
		
		color *= colorAlpha.xyz;
	
		return float4(color, 1.0);
	}

	float3 color = float3(0, 0, 0);
	float ralpha = 1.0;
	    
	uint x1 = min(input.maskcoord.x, width - 1);
	uint y1 = min(input.maskcoord.y, height - 1);
	
	uint x2 = min(x1 + 1, width - 1);
	uint y2 = min(y1 + 1, height - 1);
	
	uint size = width * height;
	
	uint4 offset = uint4(y1 * width + x1,
	                     y1 * width + x2,
	                     y2 * width + x1,
	                     y2 * width + x2);
	
	float fx = input.maskcoord.x - x1;
	float fy = input.maskcoord.y - y1;
	
	float rfx = 1.0 - fx;
	float rfy = 1.0 - fy;
	
	float4 weights = float4(rfx * rfy, fx * rfy, rfx * fy, fx * fy);
	
	float3 norm = normalize(input.normal);
		
	for (int i = layernum - 1; i >= 0; i--)
	{				
		uint baseptr = size * i;
		
		float4 values = float4(layer_masks[baseptr + offset[0]],
		                       layer_masks[baseptr + offset[1]],
		                       layer_masks[baseptr + offset[2]],
		                       layer_masks[baseptr + offset[3]]);
		
		float alpha = dot(values, weights);
					  
		if (WaveActiveAllTrue(alpha < 0.001)) continue;
					
		uint material = layers[i * 2];
		uint type = layers[i * 2 + 1];
		uint diffuse = materials[material].diffuse_map;
		
		if (type == 0)
			color += image[diffuse].Sample(g_sampler, input.tcoord).xyz * alpha * ralpha;
		else
			color += sampleTriplanar(diffuse, input.worldpos, norm) * alpha * ralpha;
		
		ralpha *= 1.0 - alpha;
		
		if (WaveActiveAllTrue(alpha > 0.999)) return float4(color, 1.0);
	}
	
	uint diffuse = materials[baseMaterial].diffuse_map;
	
	if (baseLayer == 0)
		color += image[diffuse].Sample(g_sampler, input.tcoord).xyz * ralpha;
	else
		color += sampleTriplanar(diffuse, input.worldpos, norm) * ralpha;
	
	return float4(color, 1.0);
}
