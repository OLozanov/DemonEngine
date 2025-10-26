#define blocksize 8

cbuffer Parameters : register(b0)
{
    uint2 dim;
};

RWTexture2D<half4> output : register(u0);
Texture2D<float3> input : register(t0);
Texture2D<float> depth : register(t1);

static const float Kernel[5] = { 1.0 / 16, 1.0 / 4, 3.0 / 8, 1.0 / 4, 1.0 / 16 };

float ExtractZCoord(float d)
{
    const float far = 5000.0;
    const float near = 0.1;

    const float A = far / (far - near);
    const float B = -far * near / (far - near);

    return B / (d - A);
}

bool withinBounds(uint2 pos)
{
    return (pos.x >= 0 && pos.x <= dim.x &&
            pos.y >= 0 && pos.y <= dim.y);
}

[numthreads(blocksize, blocksize, 1)]
void main( uint3 DTid : SV_DispatchThreadID )
{
    float3 value = 0;
    float3 quotient = 0;

    float d = depth[DTid.xy];
    float z = ExtractZCoord(d);
    //float3 norm = GetNormal(d, z, DTid.xy);

    float dx = depth[DTid.xy + uint2(1, 0)];
    float dy = depth[DTid.xy + uint2(0, 1)];

    float3 world_dy = float3(1.0 / dim.x, 0, ExtractZCoord(dx) - z);
    float3 world_dx = float3(0, 1.0 / dim.y, ExtractZCoord(dy) - z);

    float3 norm = normalize(cross(world_dx, world_dy));

    [unroll]
    for (int i = -2; i < 2; i++)
    {
        [unroll]
        for (int k = -2; k < 2; k++)
        {
            uint2 pos = uint2(DTid.x + i, DTid.y + k);

            if (!withinBounds(pos)) continue;
            
            float d = depth[pos];
            float loc_z = ExtractZCoord(d);

            float zweight = saturate(1.0 - abs(z - loc_z) / 0.1 / z);

            //float3 loc_norm = GetNormal(d, loc_z, pos);
            float dx = depth[pos + uint2(1, 0)];
            float dy = depth[pos + uint2(0, 1)];

            float3 world_dy = float3(1.0 / dim.x, 0, ExtractZCoord(dx) - loc_z);
            float3 world_dx = float3(0, 1.0 / dim.y, ExtractZCoord(dy) - loc_z);

            float3 loc_norm = normalize(cross(world_dx, world_dy));

            float nweight = pow(saturate(dot(norm, loc_norm)), 128);

            float weight = Kernel[i + 2] * Kernel[k + 2] * zweight * nweight;
            value += input[pos] * weight;
            quotient += weight;
        }
    }

    output[DTid.xy] = half4(value / quotient, 1.0);
    //output[DTid.xy] = half4(input[DTid.xy], 1.0);
}