#include "Random.hlsl"

struct ViewConstantBuffer
{
    float3 topleft;
    float3 xdir;
    float3 ydir;
    float3 eyepos;
    float3 prev_topleft;
    float3 prev_xdir;
    float3 prev_ydir;
    float3 prev_eyepos;
    uint frame;
    float3 sky_color;
    float3 light_color;
    uint enable_dir_light;
    float4x4 reprojectionMat;
};

struct ShadowData
{
    float4x4 shadowMat[3];
};

RaytracingAccelerationStructure scene : register(t0, space0);

ConstantBuffer<ViewConstantBuffer> cb : register(b0);
ConstantBuffer<ShadowData> shadowData : register(b1);

RWTexture2D<half4> renderTarget : register(u0);
RWTexture2D<uint> spp : register(u1);
Texture2D<float> depth : register(t1);
Texture2D<float3> prevFrame : register(t2);
Texture2D<uint> prevspp : register(t3);
Texture2D<float> prev_depth : register(t4);
Texture2D normal : register(t5);
Texture2D prev_normal : register(t6);

Texture2D<float3> noise_mask : register(t7);

Texture2DArray shadow : register(t8);

StructuredBuffer<float3> hemiSamples : register(t9);

SamplerState clampSampler : register(s0);
SamplerComparisonState  cmpSampler : register(s1);

typedef BuiltInTriangleIntersectionAttributes Attributes;

struct RayPayload
{
    float3 color;
};

float ExtractZCoord(float d)
{
    const float far = 5000.0;
    const float near = 0.1;

    const float A = far / (far - near);
    const float B = -far * near / (far - near);

    return B / (d - A);
}

float3 GetWorldPos(uint2 screen_pos, float z)
{
    float3 screen_vec = cb.topleft + cb.xdir * (screen_pos.x + 0.5) + cb.ydir * (screen_pos.y + 0.5);
    return cb.eyepos + screen_vec * z;
}

float3 GetPrevWorldPos(float2 screen_pos, float z)
{
    float3 screen_vec = cb.prev_topleft + cb.prev_xdir * (screen_pos.x + 0.5) + cb.prev_ydir * (screen_pos.y + 0.5);
    return cb.prev_eyepos + screen_vec * z;
}

float2 reproject(in float3 pos)  // return clip coords 
{
    float4 prevPos = mul(cb.reprojectionMat, float4(pos, 1.0));
    prevPos.xyz /= prevPos.w;

    return float2(prevPos.x + 1.0, 1.0 - prevPos.y) * 0.5;
}

float hash(float3 p)
{
    p = frac(p * 0.3183099 + .1);
    p *= 17.0;
    return frac(p.x * p.y * p.z * (p.x + p.y + p.z));
}

float3 TraceIncidentRays(in float3 pos, in float3 normal)
{
    const float pi = 3.14;

    float3 right = 0.3f * normal + float3(-0.72f, 0.56f, -0.34f);
    float3 tangent = normalize(cross(normal, right));
    float3 binormal = normalize(cross(tangent, normal));

    /*float2 ang = Rand_2_10();

    float w = sin(ang.y * pi * 0.5);

    float u = cos(ang.x * pi * 2.0);
    float v = sin(ang.x * pi * 2.0);*/

    uint2 screen_pos = DispatchRaysIndex().xy;

    /*uint rstate = InitSeed((screen_pos.y * 64 + screen_pos.x) * hash(pos) + cb.frame % 437585);
    uint samp = Random(rstate, 0, 1023);

    float3 dir = hemiSamples[samp];*/

    float2 noiseValue = noise_mask[screen_pos % 64].xy;
    float3 dir;

    //dir.xy = frac(noiseValue + 0.61803398875 * (cb.frame % 64));
    //dir.z = sqrt(1.0 - dir.x * dir.x - dir.y * dir.y);

    noiseValue = frac(noiseValue + 0.61803398875 * (cb.frame % 64));

    float ang = noiseValue.x * 2.0 * 3.14;
    float len = noiseValue.y;

    dir.x = cos(ang) * len;
    dir.y = sin(ang) * len;
    dir.z = sqrt(1.0 - len * len);

    RayPayload payload = { float3(0, 0, 0) };

    RayDesc ray;
    ray.Origin = pos + normal * 0.01;
    ray.Direction = normal * dir.z + tangent * dir.x + binormal * dir.y;
    ray.TMin = 0.001;
    ray.TMax = 50.0;

    TraceRay(scene, RAY_FLAG_CULL_BACK_FACING_TRIANGLES, ~0, 0, 0, 0, ray, payload);

    return payload.color;
}

[shader("raygeneration")]
void RaygenShader()
{
    float2 dim = DispatchRaysDimensions().xy;
    float2 rdim = 1.0 / dim;

    uint2 screen_pos = DispatchRaysIndex().xy;

    float d = depth[screen_pos];

    if (d == 1.0)
    {
        spp[screen_pos] = 0;
        return;
    }

    uint2 screen_posx = uint2(screen_pos.x + 1, screen_pos.y);
    uint2 screen_posy = uint2(screen_pos.x, screen_pos.y + 1);

    float z = ExtractZCoord(d);
    float3 world_pos = GetWorldPos(screen_pos, z);

    //float3 geometry_norm = normalize(cross(world_dx, world_dy));
    float3 geometry_norm = normal[screen_pos].xyz * 2.0 - 1.0; 

    float weight;
    float3 prevColor;
    float3 incidentLight = TraceIncidentRays(world_pos, geometry_norm);

    float2 reprojected_pos = reproject(world_pos);
    //float2 prev_screen_pos = round(prev_pos * dim - 0.5);
    float2 prev_screen_pos = floor(reprojected_pos * dim - 0.5); // top-left corner

    float2 prev_pos = (prev_screen_pos + 0.5) * rdim;

    float2 pixel_offset = reprojected_pos * dim - 0.5 - prev_screen_pos;
    
    //if (false)
    if (prev_pos.x > 0.0 && prev_pos.x < 1.0 &&
        prev_pos.y > 0.0 && prev_pos.y < 1.0)
    {
        const int2 index_offsets[4] = { {0, 0}, {1, 0}, {0, 1}, {1, 1} };

        float4 bilinear_weights = float4((1.0 - pixel_offset.x) * (1.0 - pixel_offset.y),
                                         pixel_offset.x * (1.0 - pixel_offset.y),
                                         (1.0 - pixel_offset.x) * pixel_offset.y,
                                         pixel_offset.x * pixel_offset.y);

        float4 d = prev_depth.GatherRed(clampSampler, prev_pos).wzxy;

        float4 pos_weights;
        float4 norm_weights;

        [unroll]
        for (int i = 0; i < 4; i++)
        {
            uint2 offset_screen_pos = prev_screen_pos + index_offsets[i];

            //  position weights
            float prevz = ExtractZCoord(d[i]);
            float3 prev_world_pos = GetPrevWorldPos(offset_screen_pos, prevz);
            pos_weights[i] = saturate(1.0 - length(world_pos - prev_world_pos) / 0.1 / z);
        
            // normal weights
            //float3 prev_norm = GetPrevNormal(d[i], prev_world_pos, offset_screen_pos);
            float3 prev_norm = prev_normal[offset_screen_pos].xyz * 2.0 - 1.0;

            norm_weights[i] = pow(saturate(dot(geometry_norm, prev_norm)), 128);
        }

        float4 weights = bilinear_weights * pos_weights * norm_weights;

        float weight_sum = dot(1, weights);

        if (weight_sum > 0.001)
        {
            weights /= weight_sum;

            /*uint4(prevspp[prev_screen_pos + index_offsets[0]],
                prevspp[prev_screen_pos + index_offsets[1]],
                prevspp[prev_screen_pos + index_offsets[2]],
                prevspp[prev_screen_pos + index_offsets[3]]);*/

            uint4 spp_vec = max(1, prevspp.GatherRed(clampSampler, prev_pos).wzxy);
            spp_vec = max(1, spp_vec);
            float spp_val = dot(weights, spp_vec);

            uint sampnum = min(round(spp_val) + 1, 30);

            //uint sampnum = min(prevspp[round(prev_screen_pos + pixel_offset)] + 1, 30);
            spp[screen_pos] = sampnum;

            float4 red = prevFrame.GatherRed(clampSampler, prev_pos).wzxy;
            float4 green = prevFrame.GatherGreen(clampSampler, prev_pos).wzxy;
            float4 blue = prevFrame.GatherBlue(clampSampler, prev_pos).wzxy;

            prevColor.x = dot(weights, red);
            prevColor.y = dot(weights, green);
            prevColor.z = dot(weights, blue);

            if (isnan(prevColor.x)) prevColor.x = 1.0;
            if (isinf(prevColor.x)) prevColor.x = 1.0;

            if (isnan(prevColor.y)) prevColor.y = 1.0;
            if (isinf(prevColor.y)) prevColor.y = 1.0;

            if (isnan(prevColor.z)) prevColor.z = 1.0;
            if (isinf(prevColor.z)) prevColor.z = 1.0;

            weight = 1.0 / sampnum;
        }
        else
        {
            spp[screen_pos] = 1;
            weight = 1.0;
        }
    }
    else
    {
        spp[screen_pos] = 1;
        weight = 1.0;
    }

    float3 result = lerp(prevColor, incidentLight, weight);
    
    renderTarget[DispatchRaysIndex().xy] = half4(result, 1.0);
}

[shader("closesthit")]
void ClosestHitShader(inout RayPayload payload, in Attributes attr)
{
    //payload.color = float3(0.0, 0.0, 0.0);

    const float max_dist = 5.0;

    if (!cb.enable_dir_light || RayTCurrent() > max_dist)
    {
        payload.color = float3(0.0, 0.0, 0.0);
        return;
    }

    float3 pos = WorldRayOrigin() + WorldRayDirection() * RayTCurrent();

    float2 ltpos = mul(shadowData.shadowMat[0], float4(pos, 1.0)).xy;
    
    int shadowLod = 1;
    
    if (abs(ltpos.x) > 2.95 || abs(ltpos.y) > 2.95)
    {
        shadowLod = 2;
    }

    float4 shadPos = mul(shadowData.shadowMat[shadowLod], float4(pos, 1.0));
    shadPos.xy = shadPos.xy * 0.5 + 0.5;
    shadPos.y = -shadPos.y;
    shadPos.z -= 0.0001;

    float att = max(0.0, max_dist - RayTCurrent()) / max_dist;
    float shad = shadow.SampleCmpLevelZero(cmpSampler, float3(shadPos.x, shadPos.y, shadowLod), shadPos.z);

    const float3 zero = float3(0.0, 0.0, 0.0);

    payload.color = lerp(zero, cb.light_color, shad * att);
}

[shader("miss")]
void MissShader(inout RayPayload payload)
{
    payload.color = cb.sky_color;
}