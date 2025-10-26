#pragma once

#include "Render/Render.h"

#include <vector>

struct SpriteData
{
    vec3 position;
    ImageHandle image;
    vec4 color;
    float size;
};

enum RenderType
{
    rc_regular = 0,
    rc_color = 1,
    rc_line = 2,
    rc_count
};

class RenderContext
{
public:
    Render::CommandList& commandList(size_t type) { return m_commandLists[type]; }
    const std::vector<SpriteData>& sprites() const { return m_sprites; }

    void addSprite(const vec3& position, ImageHandle id, float size, const vec3& color) { m_sprites.push_back({ position, id, color, size }); }

private:
    Render::CommandList m_commandLists[3];
    std::vector<SpriteData> m_sprites;
};
