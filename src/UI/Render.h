#pragma once

#ifdef RENDER_D3D

#include "UI/D3D/UiRenderer.h"
#include "Render/D3D/PushBuffer.h"

#undef min
#undef max

#endif

#include "Resources/Image.h"
#include "math/math3d.h"

namespace UI
{

struct Vertex2d
{
    vec2 position;
    vec2 texcoord;
    vec4 color;

    Vertex2d(float x, float y, const vec4& vcolor) : position({ x, y }), color(vcolor) {}
    Vertex2d(const vec2& pos, const vec4& vcolor) : position(pos), color(vcolor) {}
    Vertex2d(float x, float y, float u, float v) : position({ x, y }), texcoord({ u, v }), color{ 1.0f, 1.0f, 1.0f, 1.0f } {}
    Vertex2d(float x, float y, float u, float v, const vec4& vcolor) : position({x, y}), texcoord({u, v}), color{vcolor} {}
    Vertex2d(const vec2& pos, const vec2& tcoord, const vec4& vcolor) : position(pos), texcoord(tcoord), color(vcolor) {}
};

struct DrawCommand
{
    uint16_t vnum;
    uint8_t topology;
    uint32_t flags;
    float param;
    uint32_t imgHandle;

    DrawCommand(uint16_t vnum, uint8_t topology = UiRenderer::topology_triangle_strip)
    : vnum(vnum)
    , topology(topology)
    , flags(0)
    , param(0)
    , imgHandle(BlankImage)
    {
    }

    DrawCommand(uint16_t vnum, const Image* img, uint8_t topology = UiRenderer::topology_triangle_strip)
    : vnum(vnum)
    , topology(topology)
    , flags(0)
    , param(0)
    , imgHandle(img->handle)
    {
    }

    DrawCommand(uint16_t vnum, uint32_t handle, uint8_t topology, uint32_t flags = 0, float param = 0)
    : vnum(vnum)
    , topology(topology)
    , flags(flags)
    , param(param)
    , imgHandle(handle)
    {
    }
};

using VertexBuffer = Render::PushBuffer<Vertex2d>;
using CommandList = std::vector<DrawCommand>;

} //namespace ui