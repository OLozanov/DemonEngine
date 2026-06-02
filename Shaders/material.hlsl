
static const uint DiffuseMap = 1;
static const uint NormalMap = 2;
static const uint RoughnessMap = 4;
static const uint MetalnessMap = 8;
static const uint LuminosityMap = 0x10;
static const uint HeightMap = 0x20;

static const uint InvalidImage = 0xFFFFFFFF;

struct Material
{
    float4 color;
    float metalness;
    float roughness;
    float luminosity;
    uint flags;
	
	uint diffuse_map;
    uint normal_map;
    uint roughness_map;
    uint metalness_map;
    uint luminosity_map;
    uint height_map;
    uint ao_map;
};