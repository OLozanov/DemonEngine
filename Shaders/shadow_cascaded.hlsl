cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 shadowMat[3];
};

cbuffer ObjectConstantBuffer : register(b1)
{
    float4x4 modelMat;
};

struct GSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 tcoord : TEXCOORD;
    uint lod : SV_RenderTargetArrayIndex;
};

Texture2D image : register(t0);
SamplerState g_sampler : register(s0);

GSInput VSMain(float4 position : POSITION, float2 tcoord : TEXCOORD)
{
    GSInput result;

    result.position = mul(modelMat, position);
    result.tcoord = tcoord;
    
    return result;
}

[maxvertexcount(3*6)]
void GSMain(triangle GSInput input[3], inout TriangleStream<PSInput> OutputStream)
{   
    PSInput output;

    for(int l = 0; l < 3; l++)
    {
        for(int i = 0; i < 3; i++)
        {           
            output.position = mul(shadowMat[l], input[i].position);
            output.tcoord = input[i].tcoord;
            output.lod = l;
            
            OutputStream.Append(output);
        }
        
        OutputStream.RestartStrip();
    }
}

float4 PSMain(PSInput input) : SV_TARGET
{
    //float3 color = pow(image.Sample(g_sampler, input.tcoord), 2.2);
   
    return float4(1.0, 0.0, 0.0, 1.0);
}
