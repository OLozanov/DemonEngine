static const float pi = 3.1415;

float3 CorrectNormal(float3 n, float3 v)
{
	float NdotV = dot(n, v);
	n += (2.0 * saturate(-NdotV)) * v;

	return n;
}

float NormalDistribution(float3 n, float3 h, float a)
{   
    float a2 = a*a;
    float NdotH = max(dot(n, h), 0.0);
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
  
float GeometryFunction(float3 n, float3 v, float3 l, float a)
{
    float k = (a + 1)*(a + 1)/8.0;

    float NdotV = max(dot(n, v), 0.0);
    float NdotL = max(dot(n, l), 0.0);
    float ggx1 = GeometrySchlickGGX(NdotV, k);
    float ggx2 = GeometrySchlickGGX(NdotL, k);
	
    return ggx1*ggx2;
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

	n = CorrectNormal(n, v);

    float ndf = NormalDistribution(n, h, roughness);
    float geom = GeometryFunction(n, v, l, roughness);
    
    float3 f0 = lerp(0.04, diffuse, metallic);
    
    float3 fresnel = FresnelSchlick(max(0.0, dot(h, v)), f0);
    
    float specDenom = 4.0*max(0.0, dot(v, n));
    float3 specular = ndf*geom/max(specDenom, 0.01);

    float3 kS = fresnel * (1.0 - max(roughness - 0.9, 0.0) * 10.0);
    float3 kD = max(1.0 - kS, 0.0);
    kD *= 1.0 - metallic;
    
    return (kD*diffuse/pi*ltCos + kS*specular)*radiance;
}