#pragma once

#include "Resources/Image.h"
#include "Resources/Material.h"
#include "Resource.h"
#include <string>

using ImageList = std::vector<ResourcePtr<Image>>;

class AnimatedMap
{
    Material* m_material;
    int m_map;

    float m_time;
    float m_duration;

    ImageList m_images;

    int m_frame;

    static constexpr float AnimRate = 0.05f;

public:
    AnimatedMap(Material * material, int mapnum, std::string name);
    
    size_t referenceCount();
    void update(float dt);
};
