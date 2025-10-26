#define blocksize 32

cbuffer Parameters : register(b0)
{
    float3 ambient_color;
    float pad;
    float3 background_color;
};

RWTexture2D<half4> renderTarget : register(u0);
Texture2D<float3> ambient : register(t0);
Texture2D<float3> diffuse : register(t1);
Texture2D<float3> params : register(t2);

[numthreads(blocksize, blocksize, 1)]
void CSMain(uint3 Gid : SV_GroupID, uint3 DTid : SV_DispatchThreadID, uint3 GTid : SV_GroupThreadID, uint GI : SV_GroupIndex)
{
    //const float3 factor = float3(0.05, 0.05, 0.05);
    
    float luminance = params[DTid.xy].z;
    
    float3 diffcolor = pow(diffuse[DTid.xy], 2.2);
    float3 ambcolor = ambient[DTid.xy] * ambient_color + background_color;
    float3 color = max(diffcolor * luminance, diffcolor * ambcolor);

    renderTarget[DTid.xy] += half4(color, 1.0);
}

