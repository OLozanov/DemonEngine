static const float2 quad[4] = {{ -1.0f, 1.0f },
                               { 1.0f, 1.0f },
                               { -1.0f, -1.0f },
                               { 1.0f, -1.0f }};
                               
cbuffer SceneConstantBuffer : register(b0)
{
    float4x4 projViewMat;
    float4x4 worldMat;
    float3 eyepos;
    float fovx;
    float fovy;
    float padding[27];
};

cbuffer LightData : register(b1)
{
    float3 light_pos;
    float light_radius;
    float3 light_flux;
    float light_falloff;
};

Texture2D diffuse : register(t0);
Texture2D normal : register(t1);
Texture2D parameters : register(t2);
Texture2D depth : register(t3);

struct PSInput
{
    float4 position : SV_POSITION;
    float3 clip_pos : CLIP_POS;
};

static const float pi = 3.1415;

PSInput VSMain(uint vid: SV_VertexID)
{
    PSInput result;
    
    float3 clip_pos;
    
    float3 ltvec = light_pos - eyepos;    
    float ltsize = light_radius + light_falloff;
    
    float4 position = float4(quad[vid].x, quad[vid].y, 1.0, 1.0);
    
    if(dot(ltvec, ltvec) < ltsize*ltsize + 0.25)
    {
        result.position = position;
        clip_pos = float3(position.x*fovx, position.y*fovy, 1.0);
    }
    else
    {
        float4 pos = mul(projViewMat, float4(light_pos, 1.0));
        
        float screen_size = (light_radius + light_falloff + 0.5)/pos.w*fovx;
        float2 screen_pos = float2(position.x/fovx, position.y/fovy)*screen_size + pos.xy/pos.w;
        
        result.position = float4(screen_pos, pos.w > -ltsize ? 1.0 : -1.0, 1.0);
    
        clip_pos = float3(screen_pos.x*fovx, screen_pos.y*fovy, 1.0);
    }
       
    result.clip_pos = mul(worldMat, clip_pos);
    
    return result;
}

float ExtractZCoord(float d)
{
    const float far = 5000.0;
    const float near = 0.1;
    
    const float A = far/(far - near);
    const float B = -far*near/(far - near);
    
    return B/(d - A);
    //return near * far / (far - d * (far - near));
}

float NormalDistribution(float3 norm, float3 halfvec, float a)
{   
    float a2 = a*a;
    float NdotH = max(dot(norm, halfvec), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float denom = NdotH2*(a2 - 1.0) + 1.0;
    denom = pi*denom*denom;
	
    return a2/denom;
}

float GeometrySchlickGGX(float NdotV, float k)
{
    float denom = NdotV*(1.0 - k) + k;
    return NdotV/denom;
}
  
float GeometryFunction(float3 norm, float3 view, float3 lvec, float a)
{
    float k = (a + 1)*(a + 1)/8.0;
    
    float NdotV = max(dot(norm, view), 0.0);
    float NdotL = max(dot(norm, lvec), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1*ggx2;
}

float3 FresnelSchlick(float cosTheta, float3 f0)
{
    return f0 + (1.0 - f0)*pow(max(1.0 - cosTheta, 0.0), 5.0);
}

float4 PSMain(PSInput input) : SV_TARGET
{
    int3 screen_pos = int3(input.position.xy, 0);
    
    float3 diffuse_color = pow(diffuse.Load(screen_pos).rgb, 2.2);
    float4 params = parameters.Load(screen_pos);
    
    float3 norm = 2.0*normal.Load(screen_pos) - 1.0;
    norm = normalize(norm);
    
    float z = ExtractZCoord(depth.Load(screen_pos));
    
    float3 clip_vec = input.clip_pos*z;
    float3 world_pos = clip_vec + eyepos; 
    
    float3 lvec = light_pos - world_pos;
	float3 ldir = normalize(lvec);
	float dist = length(lvec);
       
    float ltCos = max(0.0, dot(ldir, norm));
    
    //specular
	float3 eyeVec = normalize(-clip_vec);
	float3 halfvec = normalize(ldir + eyeVec);
    
	float ndf = NormalDistribution(norm, halfvec, params[1]);
    float geom = GeometryFunction(norm, eyeVec, ldir, params[1]);
    
    //attenuation
	float att;
	if(dist <= light_radius) att = 1.0;
	else if (dist <= light_radius + light_falloff) att = 1.0 - (dist - light_radius)/light_falloff;
	else att = 0.0;
    
    float f0 = lerp(0.04, diffuse_color, params[0]);
    
    float3 kS = FresnelSchlick(max(0.0, dot(halfvec, eyeVec)), f0);
    float3 kD = 1.0 - kS;
    kD *= 1.0 - params[0];
    
    float specDenom = 4.0*ltCos*max(0.1, dot(eyeVec, norm));
    float3 specular = ndf*geom/max(specDenom, 0.01);
    
    float3 radiance = light_flux*att*4;
    float3 color = (kD*diffuse_color/pi + kS*specular)*ltCos*radiance;
    
    //color = color/(color + 1.0);
    //color = pow(color, 1.0/2.2);    
    
    return float4(color, 1.0);
}
