cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float padding[48];
};

cbuffer SkyboxConstantBuffer : register(b1)
{
    uint4 faces[2];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
};

Texture2D depth : register(t0);
Texture2D image[] : register(t1);
SamplerState g_sampler : register(s0);

PSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD)
{
    PSInput result;

    result.position = mul(projViewMat, position); //mul(projViewMat, mul(modelMat, position));
    result.tcoord = tcoord;
    
    return result;
}

float4 PSMain(PSInput input, uint pid : SV_PrimitiveID) : SV_TARGET
{
	uint face = pid / 2;
	uint i = face / 4;
	uint k = face % 4;
	uint img = faces[i][k];
	
    int3 screen_pos = int3(input.position.xy, 0);

    float z = depth.Load(screen_pos);
    if(z < 1.0) discard;
	
    float3 color = pow(image[img].Sample(g_sampler, input.tcoord), 2.2);    
    return float4(color, 1.0);
}
