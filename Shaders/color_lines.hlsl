static const float2 quad[4] = {{ -1.0f, 1.0f },
                               { 1.0f, 1.0f },
                               { -1.0f, -1.0f },
                               { 1.0f, -1.0f }};
                               
static const float blend_width = 1.0;

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 eyepos;
    float fovx;
    float fovy;
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

cbuffer ParamsConstantBuffer : register(b3)
{
    float line_width;
};

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv : UV;
    nointerpolation float length : LENGTH;
};

struct GSInput
{
    float4 position : SV_POSITION;
};

GSInput VSMain(float4 position : POSITION)
{
    GSInput result;
    result.position = mul(modelMat, position);

    return result;
}

[maxvertexcount(4)]
void GSMain(line GSInput input[2], inout TriangleStream<PSInput> OutputStream)
{
    const float znear = 0.1f;
    const float zfar = 5000.0f;
    
    const float depth_bias = 20.0 / (zfar - znear);
    
    PSInput output;
    
    float4 p1 = mul(projViewMat, input[0].position);
    float4 p2 = mul(projViewMat, input[1].position);
    
    if (p1.z < 0 && p2.z < 0) return;
          
    float4 dir = normalize(p2 - p1);  
    
    if (p1.z < 0) p1 += dir * (abs(p1.z) / dir.z);
    if (p2.z < 0) p2 += dir * (abs(p2.z) / dir.z);

    p1.xyz /= p1.w;
    p2.xyz /= p2.w;
    
    float2 edge = p2.xy - p1.xy;
    float len = length(float2(edge.x * fovx, edge.y * fovy));
    
    float2 screen_dir = normalize(edge);
    float2 side = float2(-screen_dir.y / fovx, screen_dir.x / fovy);
    screen_dir.x /= fovx;
    screen_dir.y /= fovy;
    
    float width = line_width + blend_width;
    
    float screen_width = length(screen_dir * width);

    output.position = float4(p2.xy - side * width + screen_dir * (width - 1), p2.z - depth_bias / p2.w, 1.0);
    output.uv = float2(-width, len + width);
    output.length = len;
    OutputStream.Append(output);
    
    output.position = float4(p2.xy + side * width + screen_dir * (width - 1), p2.z - depth_bias / p2.w, 1.0);
    output.uv = float2(width, len + width);
    output.length = len;
    OutputStream.Append(output);
    
    output.position = float4(p1.xy - side * width - screen_dir * (width - 1), p1.z - depth_bias / p1.w, 1.0);
    output.uv = float2(-width, -width);
    output.length = len;
    OutputStream.Append(output);
       
    output.position = float4(p1.xy + side * width - screen_dir * (width - 1), p1.z - depth_bias / p1.w, 1.0);
    output.uv = float2(width, -width);
    output.length = len;
    OutputStream.Append(output);
    
    OutputStream.RestartStrip();
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float width = line_width + blend_width;
    float len = input.length;
    
    float end_dir = input.uv - float2(0, len);
    
    float u = input.uv.y < 0 ? length(input.uv) :
              (input.uv.y > len ? length(input.uv - float2(0, len)) :
              abs(input.uv.x));
              
    if (u > width) discard;
    
    float alpha = min(1.0, (width - u) / (blend_width * 2.0));
        
    return float4(colorAlpha.xyz, alpha);
}
