static const float4x4 FaceMat[6] = {
                        {{0, 0, -1, 0},
                         {0, 1, 0, 0},
                         {1, 0, 0, 0},
                         {0, 0, 0, 1}},
                        
                        {{0, 0, 1, 0},
                         {0, 1, 0, 0},
                         {-1, 0, 0, 0},
                         {0, 0, 0, 1}},
                          
                        {{1, 0, 0, 0},
                         {0, 0, -1, 0},
                         {0, 1, 0, 0},
                         {0, 0, 0, 1}},
                        
                        {{1, 0, 0, 0},
                         {0, 0, 1, 0},
                         {0, -1, 0, 0},
                         {0, 0, 0, 1}},
                        
                        {{1, 0, 0, 0},
                         {0, 1, 0, 0},
                         {0, 0, 1, 0},
                         {0, 0, 0, 1}},
                        
                        {{-1, 0, 0, 0},
                         {0, 1, 0, 0},
                         {0, 0, -1, 0},
                         {0, 0, 0, 1}}
                        };

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projectionMat;
    float padding[48];
};

cbuffer ObjectData : register(b1)
{
    float4x4 modelMat;
};

cbuffer CubemapData : register(b2)
{
    float3 cubeCenter;
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
    uint face : SV_RenderTargetArrayIndex;
};

SamplerState g_sampler : register(s0);

GSInput VSMain(float4 position : POSITION, float4 tcoord : TEXCOORD)
{
    GSInput result;

    result.position = mul(modelMat, position) - float4(cubeCenter, 0.0);
    result.tcoord = tcoord;

    return result;
}

[maxvertexcount(3*6)]
void GSMain(triangle GSInput input[3], inout TriangleStream<PSInput> OutputStream)
{   
    PSInput output;

    for(int f = 0; f < 6; f++)
    {
        for(int i = 0; i < 3; i++)
        {           
            output.position = mul(projectionMat, mul(FaceMat[f], input[i].position));
            output.tcoord = input[i].tcoord;
            output.face = f;
            
            OutputStream.Append(output);
        }
        
        OutputStream.RestartStrip();
    }
}

float4 PSMain(PSInput input) : SV_TARGET
{
    return float4(1.0, 0.0, 0.0, 1.0);
}