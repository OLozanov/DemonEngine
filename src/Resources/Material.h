#pragma once

#include "math/math3d.h"
#include "Resources/Resource.h"
#include "Resources/Image.h"

#include <vector>
#include <string>

class Model;

struct Material : public Resource
{
    enum MapId
    {
        map_diffuse = 0,
        map_normal = 1,
        map_roughness = 2,
        map_metalness = 3,
        map_luminosity = 4,
        map_height = 5,
        map_count
    };
    
    enum MaterialType
    {
        material_regular,
        material_transparent,
        material_emissive
    };

    enum MaterialFlags : uint32_t
    {
        mat_diffuse_map = 1,
        mat_normal_map = 2,
        mat_roughness_map = 4,
        mat_metalness_map = 8,
        mat_luminosity_map = 0x10,
        mat_height_map = 0x20
    };

    std::string name;
    MaterialType type;
    
    //Maps
    ResourcePtr<Image> img[map_count];
    UINT maps[map_count];
    
    //parameters
    vec3 color;
    float alpha;
    float metalness;
    float roughness;
    float luminosity;
    uint32_t flags;

    Material()
    : type(material_regular)
    , color(1.0, 1.0, 1.0)
    , alpha(1.0)
    , metalness(0.0)
    , roughness(0.8)
    , luminosity(0)
    , flags(0)
    {
    }

    ~Material();
};
