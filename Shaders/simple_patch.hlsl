#include "material.hlsl"

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float4x4 worldMat;
    float3 eyepos;
	float padding[27];
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

struct Vertex
{
	float3 position;
	float2 tcoord;
	float3 normal;
};

struct HSInput { float3 position : POSITION; };
struct HSControlPoint { float3 position : POSITION; };

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
StructuredBuffer<Vertex> vbuffer : register(t3);

Texture2D image[] : register(t4);
SamplerState g_sampler : register(s0);

HSInput VSMain(float3 position : POSITION)
{
    HSInput result;

    result.position = position;
    return result;
}

struct HSConstantData 
{
    float EdgeFactor[4]   : SV_TessFactor;
    float InsideFactor[2] : SV_InsideTessFactor;
};

float TesselationFactor(float3 pt) 
{
	const float baseDist = 4;
	
	float dist = max(baseDist, length(eyepos - pt));
	float lod = width / (log2(dist / baseDist) + 1.0);
	
	return clamp(lod, 4, width);
}

HSConstantData PatchConstantFunc(InputPatch<HSInput, 4> patch) 
{
    HSConstantData result;
	
	float3 e0 = 0.5f * (patch[0].position + patch[2].position);
    float3 e1 = 0.5f * (patch[0].position + patch[1].position);
    float3 e2 = 0.5f * (patch[1].position + patch[3].position);
    float3 e3 = 0.5f * (patch[2].position + patch[3].position);
    float3 c = 0.25f * (patch[0].position + patch[1].position + patch[2].position + patch[3].position);
	
	result.EdgeFactor[0] = TesselationFactor(e0);
    result.EdgeFactor[1] = TesselationFactor(e1);
    result.EdgeFactor[2] = TesselationFactor(e2);
    result.EdgeFactor[3] = TesselationFactor(e3);
    result.InsideFactor[0] = TesselationFactor(c);
    result.InsideFactor[1] = result.InsideFactor[0];
	
    return result;
}

[domain("quad")]
[partitioning("fractional_even")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(4)]
[patchconstantfunc("PatchConstantFunc")]
HSControlPoint HSMain(InputPatch<HSInput, 4> patch, uint i : SV_OutputControlPointID) 
{
    HSControlPoint result;
    result.position = patch[i].position;
    return result;
}

[domain("quad")]
PSInput DSMain(HSConstantData input, float2 uv : SV_DomainLocation, const OutputPatch<HSControlPoint, 4> patch) 
{
    PSInput result;
		
	float2 maskcoord = float2(uv.x * width, uv.y * height);
    
	result.tcoord = uv;
	result.maskcoord = maskcoord;
	
	result.normal = float3(0.0, 0.0, 0.0);
	
	uint x1 = min(maskcoord.x, width - 1);
	uint y1 = min(maskcoord.y, height - 1);
	
	uint x2 = min(x1 + 1, width - 1);
	uint y2 = min(y1 + 1, height - 1);
	
	uint size = width * height;
	
	uint4 offset = uint4(y1 * width + x1,
	                     y1 * width + x2,
	                     y2 * width + x1,
	                     y2 * width + x2);
	
	float fx = maskcoord.x - x1;
	float fy = maskcoord.y - y1;
	
	float rfx = 1.0 - fx;
	float rfy = 1.0 - fy;
	
	float4 weights = float4(rfx * rfy, fx * rfy, rfx * fy, fx * fy);
	
	result.worldpos = vbuffer[offset[0]].position * weights[0] + 
					  vbuffer[offset[1]].position * weights[1] + 
					  vbuffer[offset[2]].position * weights[2] + 
					  vbuffer[offset[3]].position * weights[3];
					  
	result.tcoord = vbuffer[offset[0]].tcoord * weights[0] + 
					vbuffer[offset[1]].tcoord * weights[1] + 
					vbuffer[offset[2]].tcoord * weights[2] + 
					vbuffer[offset[3]].tcoord * weights[3];
					
	result.normal = vbuffer[offset[0]].normal * weights[0] + 
					vbuffer[offset[1]].normal * weights[1] + 
					vbuffer[offset[2]].normal * weights[2] + 
					vbuffer[offset[3]].normal * weights[3];
	
	result.position = mul(projViewMat, mul(modelMat, float4(result.worldpos, 1.0)));

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
