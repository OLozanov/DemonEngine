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

struct PSInput
{
    float4 position : SV_POSITION;
};

PSInput VSMain(float4 position : POSITION)
{
    PSInput result;

    result.position = mul(projViewMat, mul(modelMat, position));

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return colorAlpha;
}
