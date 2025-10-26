static const float pi = 3.1415;

float NormalDistribution(float3 n, float3 h, float a)
{   
    /*float a2 = a*a;
    float NdotH = max(dot(n, h), 0.0);
    float NdotH2 = NdotH*NdotH;
	
    float denom = NdotH2*(a2 - 1.0) + 1.0;
    denom = pi*denom*denom;
	
    return a2/denom;*/

    float a2 = a * a;
    
    float NdotH = max(dot(n, h), 0.0);
    float NdotH2 = NdotH * NdotH;
    
    float denom = NdotH2 * a2 + (1 - NdotH2);
    
    float chi = NdotH > 0.0 ? 1.0 : 0.0;
    
    return (chi*a2)/(pi*denom*denom);
}

float GeometryGGX(float3 n, float3 v, float3 h, float a)
{
    float VdotH = max(dot(v, h), 0.0);
    float VdotN = max(dot(v, n), 0.0);
    
    float chi = (VdotH/VdotN) > 0.0 ? 1.0 : 0.0;
    
    float VdotH2 = VdotH * VdotH;
    
    float tan2 = (1 - VdotH2)/VdotH2;
    
    return (chi * 2)/(1 + sqrt(1 + a*a * tan2));
}
  
float GeometryFunction(float3 n, float3 v, float3 l, float3 h, float a)
{
    float k = a*a;//(a + 1)*(a + 1)/8.0;
       
    float g1 = GeometryGGX(n, v, h, a);
    float g2 = GeometryGGX(n, l, h, a);
	
    return g1*g2;
}

float GeometryCookTorrance(float3 n, float3 v, float3 l, float3 h)
{
    float NdotH = max(dot(n, h), 0.0);
    float VdotH = max(dot(v, h), 0.0);
    float NdotV = max(dot(n, v), 0.0);
    float NdotL = max(dot(n, l), 0.0);
    
    float g1 = 2 * NdotH * NdotV / VdotH;
    float g2 = 2 * NdotH * NdotL / VdotH; 
    
    return min(1.0, min(g1, g2));
}

float GeometrySmith(float3 n, float3 v, float3 l, float a)
{
    float k = (a + 1)*(a + 1)/8.0;
    
    float a2 = a*a;
    
    float NdotV = max(dot(n, v), 0.0);
    float NdotL = max(dot(n, l), 0.0);
       
    float g1 = (NdotL * 2.0) / (NdotL + sqrt(k + (1.0 - k) * NdotL*NdotL));
    float g2 = (NdotV * 2.0) / (NdotV + sqrt(k + (1.0 - k) * NdotV*NdotV));
	
    return g1*g2;
}

float3 FresnelSchlick(float cosTheta, float3 f0)
{
    return f0 + (1.0 - f0)*pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

float3 BRDF(float3 n,               // surface normal
            float3 l,               // light direction
            float3 v,               // view direction
            float3 diffuse,
            float3 radiance,
            float roughness, 
            float metallic)
{
	float3 h = normalize(l + v); // half vector

    float ltCos = max(0.0, dot(l, n));

    float a = roughness * roughness;
    
    float ndf = NormalDistribution(n, h, roughness);
    float geom = GeometrySmith(n, v, l, roughness); //GeometryFunction(n, v, l, h, roughness);
    
    float f0 = lerp(0.04, diffuse, metallic);
    
    float3 fresnel = FresnelSchlick(max(0.0, dot(h, v)), f0);
    
    float specDenom = 4.0*max(0.0, dot(v, n)) * ltCos;
    float3 specular = fresnel*ndf*geom/max(specDenom, 0.01);
    
    float3 kD = max(1.0 - specular, 0.0);
    kD *= 1.0 - metallic;
    
    return (kD*diffuse/pi + specular)*ltCos*radiance;
}