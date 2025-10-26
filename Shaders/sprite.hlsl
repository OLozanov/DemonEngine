static const float2 quad[4] = {{ -1.0f, 1.0f },
                               { 1.0f, 1.0f },
                               { -1.0f, -1.0f },
                               { 1.0f, -1.0f }};
                               
static const float2 tex[4] = {{ 0.0f, 0.0f },
                               { 1.0f, 0.0f },
                               { 0.0f, 1.0f },
                               { 1.0f, 1.0f }};

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 eyepos;
    float fovx;
    float fovy;
    float padding[27];
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
    nointerpolation float4 color : COLOR;
    nointerpolation uint imageId : IMAGEID;
};

Texture2D image[] : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(uint vid: SV_VertexID, float3 position : POSITION, float size : SIZE, float4 color : COLOR, uint imageId : IMAGE)
{   
    PSInput result;

    float3 xdir = float3(worldMat[0][0], worldMat[1][0], worldMat[2][0]);
    float3 ydir = float3(worldMat[0][1], worldMat[1][1], worldMat[2][1]);
        
    float3 vertpos = position + (quad[vid].x * xdir + quad[vid].y * ydir) * size;
    result.position = mul(projViewMat, float4(vertpos, 1.0));
    result.uv = tex[vid];
    result.color = color;
    result.imageId = imageId;

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{  
    float4 imgcolor = image[input.imageId].Sample(g_sampler, input.uv) * input.color;
    
    if(imgcolor.w < 0.01) discard;
    
    return pow(imgcolor, 2.2);
}
