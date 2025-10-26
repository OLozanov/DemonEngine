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

cbuffer SpriteConstantBuffer : register(b1)
{
    float3 position;
    uint pad;
    float4 color;
    float size;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : TEXCOORD;
};

Texture2D image : register(t0);
SamplerState g_sampler : register(s0);

PSInput VSMain(uint vid: SV_VertexID)
{   
    PSInput result;

    float3 vertpos = position + (quad[vid].x * worldMat[0].xyz + quad[vid].y * worldMat[1].xyz) * size;
    result.position = mul(projViewMat, float4(vertpos, 1.0));
    result.uv = tex[vid];

    return result;
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float4 imgcolor = image.Sample(g_sampler, input.uv);
    
    if(imgcolor.w < 0.5) discard;
    
    return imgcolor * color;
}
