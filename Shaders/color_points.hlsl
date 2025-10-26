static const float2 quad[4] = {{ -1.0f, 1.0f },
                               { 1.0f, 1.0f },
                               { -1.0f, -1.0f },
                               { 1.0f, -1.0f }};

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 eyepos;
    float width;
    float height;
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

struct PSInput
{
    float4 position : SV_POSITION;
    float2 uv: UV;
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
void GSMain(point GSInput input[1], inout TriangleStream<PSInput> OutputStream)
{
    static const float point_size = 10;
    
    PSInput output;
    
    float4 pos = mul(projViewMat, input[0].position);
    pos.xy /= pos.w;
    
    if (pos.z < 0) return;
    
    pos.xy = float2(floor(pos.x * width - 0.75), floor(pos.y * height - 0.75));
    
    for(int i = 0; i < 4; i++)
    {
        float2 screen_pos = float2(quad[i].x, quad[i].y)*point_size + pos.xy;
        
        output.position = float4((screen_pos.x + 0.5) / width, (screen_pos.y + 0.5) / height, pos.z / pos.w, 1.0);
        output.uv = quad[i];
        OutputStream.Append(output);
    }
    
    OutputStream.RestartStrip();
}

float4 PSMain(PSInput input) : SV_TARGET
{
    float dist = sqrt(pow(input.uv.x, 4) + pow(input.uv.y, 4));
    
    if (dist > 1.0) discard;

    float alpha = min(0.5, 1.0 - dist) / 0.5;
    
    //return float4(abs(input.uv.x), abs(input.uv.y), 0.0, 1.0); 
    //return float4(dist, 0, 0, 1.0);
    return float4(colorAlpha.xyz, colorAlpha.w * alpha);
}
