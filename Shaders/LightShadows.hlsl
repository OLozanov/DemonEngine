//////////////////////////////////////////////////
// Functions for shadow calculation for directed
// omni and spot light sources
//////////////////////////////////////////////////

float DirToDepth(float3 dir)
{
    float3 adir = abs(dir);
    float z = max(adir.x, max(adir.y, adir.z));
	z -= 0.1;

    const float far = 500.0;
    const float near = 0.1;
    
    const float A = far/(far - near);
    const float B = -far*near/(far - near);
    
    return B/z + A;
}

float DirectedShadow(float3 world_pos, float3 norm)
{
    float2 ltpos = mul(shadowMat[0], float4(world_pos, 1.0)).xy;
    
    int shadowLod = 0;
	float blurScale = 1;
	
	if (abs(ltpos.x) > 0.95 || abs(ltpos.y) > 0.95)
	{
		shadowLod = 1;
		blurScale = 0.33;
	}
    
	if (abs(ltpos.x) > 2.95 || abs(ltpos.y) > 2.95)
    {
    	shadowLod = 2;
		blurScale = 0.33 * 0.33;
    }
    
    float zbias = max(0.0001 * (1.0 - abs(dot(norm, light_dir))), 0.0001); 
    
    float4 shadPos = mul(shadowMat[shadowLod], float4(world_pos, 1.0));
    shadPos.xy = shadPos.xy*0.5 + 0.5;
    shadPos.y = -shadPos.y;
    shadPos.z -= zbias; //0.0001;
    //float3 probe = float3(shadPos.xy, shadowLod);
    //float shad = shadow.SampleCmpLevelZero(g_cmpSampler, probe, shadPos.z);
    
    const int kernel_sz = 3;
	const int kernel_len = kernel_sz*2 + 1;
	const int probe_num = kernel_len*kernel_len;
    
    const float shadowRes = 1.0/2048;
    
    float shad = 0;
	
    [unroll]
	for(int i = -kernel_sz; i <= kernel_sz; i++)
	{
        [unroll]
		for(int k = -kernel_sz; k <= kernel_sz; k++)
		{
			float2 probe = shadPos.xy + float2(i, k)*shadowRes*blurScale;
			shad += shadow.SampleCmpLevelZero(g_cmpSampler, float3(probe.x, probe.y, shadowLod), shadPos.z);
		}
	}
	
	return shad / probe_num;
}

float OmniShadow(uint shadow_idx, float3 lvec)
{
    const int kernel_sz = 5;
    const int kernel_len = kernel_sz*2 + 1;
    const int probe_num = kernel_len*kernel_len;
    
    const float shadowRes = 1.0/1024;
    
    float3 u;
    float3 v;
    
    float lx = abs(lvec.x);
    float ly = abs(lvec.y);
    float lz = abs(lvec.z);
        
    if(abs(lx) > abs(ly) && abs(lx) > abs(lz))
    {
        u = float3(0, 0, 1)*lx;
        v = float3(0, 1, 0)*lx;
    }
    
    if(abs(ly) > abs(lx) && abs(ly) > abs(lz))
    {
        u = float3(1, 0, 0)*ly;
        v = float3(0, 0, 1)*ly;
    }
    
    if(abs(lz) > abs(lx) && abs(lz) > abs(ly))
    {
        u = float3(1, 0, 0)*lz;
        v = float3(0, 1, 0)*lz;
    }
    
    u *= shadowRes;
    v *= shadowRes;
    
    float shad = 0.0;

    float d = DirToDepth(-lvec);
           
    [unroll]
    for(int i = -kernel_sz; i <= kernel_sz; i++)
    {
        for(int k = -kernel_sz; k <= kernel_sz; k++)
        {
            float3 probe = -lvec + i*u + k*v;			
            shad += omni_shadow[shadow_idx].SampleCmpLevelZero(g_cmpSampler, probe, d);
        }
    }
    
    return shad / probe_num;
}

float SpotShadow(uint id, float3 world_pos)
{
    uint shadow_idx = spot_lights[id].shadow_idx;
    
    float4 shadPos = mul(spot_lights[id].shadow_mat, float4(world_pos, 1.0));
    shadPos.xyz /= shadPos.w;
    shadPos.xy = shadPos.xy*0.5 + 0.5;
    shadPos.y = -shadPos.y;
    shadPos.z -= 0.0001;
       
    const int kernel_sz = 5;
    const int kernel_len = kernel_sz*2 + 1;
    const int probe_num = kernel_len*kernel_len;
    
    const float shadowRes = 1.0/1048;
    
    float shad = 0.0;
   
    [unroll]
    for(int i = -kernel_sz; i <= kernel_sz; i++)
    {
        [unroll]
        for(int k = -kernel_sz; k <= kernel_sz; k++)
        {
            float2 probe = shadPos.xy + float2(i, k)*shadowRes;
            shad += spot_shadow[shadow_idx].SampleCmpLevelZero(g_cmpSampler, probe, shadPos.z);
        }
    }
    
    return shad / probe_num;
}