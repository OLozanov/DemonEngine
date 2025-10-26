#include "Light.hlsl"

#define blocksize 8
#define maxlights 128

cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
	float padding[48];
};

cbuffer Parameters : register(b1)
{
    uint xtiles;
    float xtile_sz;
    float ytile_sz;
    uint omni_num;
    uint spot_num;
};

StructuredBuffer<OmniLightData> omni_lights : register(t0);
StructuredBuffer<SpotLightData> spot_lights : register(t1);
StructuredBuffer<uint> view_lights_omni : register(t2);
StructuredBuffer<uint> view_lights_spot : register(t3);
RWStructuredBuffer<uint> light_grid : register(u0);

[numthreads(1, 1, 1)]
void CSMain(uint3 groupId : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    float3 tile_offset = float3(-2.0 * float(groupId.x) + xtile_sz - 1.0,
                                -2.0 * float(groupId.y) + ytile_sz - 1.0,
                                0);
    
    float4x4 projToTile = float4x4(xtile_sz, 0, 0, tile_offset.x,
                                   0, -ytile_sz, 0, tile_offset.y,
                                   0, 0, 1, tile_offset.z,
                                   0, 0, 0, 1);
                                   
    float4x4 tileProj = mul(projToTile, projViewMat);
    
    float4 frustum[6];
    frustum[0] = tileProj[3] + tileProj[0];
    frustum[1] = tileProj[3] - tileProj[0];
    frustum[2] = tileProj[3] + tileProj[1];
    frustum[3] = tileProj[3] - tileProj[1];
    frustum[4] = tileProj[3] + tileProj[2];
    frustum[5] = tileProj[3] - tileProj[2];
    
    for (uint n = 0; n < 6; n++)
    {
        frustum[n] *= rsqrt(dot(frustum[n].xyz, frustum[n].xyz));
    }
    
    uint onum = 0;
    uint snum = 0;
    
    uint toffset = (groupId.y * xtiles + groupId.x) * maxlights;

    for (uint i = 0; i < omni_num; i++)
    {
        uint idx = view_lights_omni[i];
        
        float radius = omni_lights[idx].radius + omni_lights[idx].falloff;
        float3 pos = omni_lights[idx].pos;
        
        bool intersect = true;
        for (uint f = 0; f < 6; f++)
        {       
            float d = dot(pos, frustum[f].xyz) + frustum[f].w;
            if (d < -radius) intersect = false;
        }
        
        if (intersect)
        {
            light_grid[toffset + onum + 1] = idx;
            onum++;
        }
    }
          
    for (uint i = 0; i < spot_num; i++)
    {
        uint idx = view_lights_spot[i];
        
        float radius = spot_lights[idx].radius + spot_lights[idx].falloff;
        float3 pos = spot_lights[idx].pos;
        
        bool intersect = true;
        for (uint f = 0; f < 6; f++)
        {       
            float d = dot(pos, frustum[f].xyz) + frustum[f].w;
            if (d < -radius) intersect = false;
        }
        
        if (intersect)
        {
            light_grid[toffset + onum + snum + 1] = idx;
            snum++;
        }
    }
    
    light_grid[toffset] = (onum & 0xff) | ((snum & 0xff) << 8);
}

