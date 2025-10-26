struct SpotLightData
{
    float3 pos;
    float radius;
    float3 flux;
    float falloff;
    float3 dir;
    float angle_outer;
    float angle_inner;
    uint shadow_idx;
    float padding[2];
    float4x4 shadow_mat;
};

struct OmniLightData
{
    float3 pos;
    float radius;
    float3 flux;
    float falloff;
    uint shadow_idx;
};

struct LightGridEntry
{
    uint offset;
    uint num;
};