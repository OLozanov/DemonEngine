struct SpotLightData
{
    float3 pos;
    float radius;
    float3 flux;
    float falloff;
    float3 dir;
	float width;
	float3 sidedir;
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

#define blocksize 16
#define maxlights 128