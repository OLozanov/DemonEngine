#pragma once

#ifdef RENDER_D3D
#include <d3d12.h>
#endif

#include <stdint.h>
#include <wrl.h>

#include "math/math3d.h"
#include "Resources/Material.h"

namespace Render
{

using Microsoft::WRL::ComPtr;

#ifdef RENDER_D3D
using VertexData = D3D12_VERTEX_BUFFER_VIEW;
using IndexData = D3D12_INDEX_BUFFER_VIEW;
#endif

struct SpriteData
{
    vec3 position;
    float size;
    vec4 color;
    uint32_t image;
};

struct Vertex
{
    vec3 position;
    vec2 tcoord;

    vec3 normal;
    vec3 tangent;
    vec3 binormal;
};

struct TexturedVertex
{
    vec3 position;
    vec2 tcoord;
};

struct GeometryData
{
    uint32_t offset;
    uint32_t count;
};

struct DisplayData
{
    ResourcePtr<Material> material;

    uint32_t offset;
    uint32_t vertexnum;
};

struct InstanceData
{
    const mat4* mat = nullptr;

    ResourcePtr<Material> material;

    const VertexData* vertexData = nullptr;
    const IndexData* indexData = nullptr;
    const VertexData* instanceData = nullptr;

    uint32_t vertexnum;
    uint32_t instancenum;
};

struct DisplayBlock
{
    enum DisplayType
    {
        display_regular,
        display_layered,
        display_decal,
        display_transparent,
        display_emissive,
        display_watersurf,
        display_sprite,
        display_debug
    };

    DisplayType type;
    bool hide = false;
    const mat4* mat = nullptr;
    const VertexData* vertexData = nullptr;
    const IndexData* indexData = nullptr;
    std::vector<const DisplayData*> displayData;
    const VertexData* layersData = nullptr;

    DisplayBlock() = default;

    DisplayBlock(DisplayType type,
                 const mat4* mat,
                 const VertexData* vertexData,
                 std::vector<const DisplayData*>& displayData)
    : type(type)
    , hide(false)
    , mat(mat)
    , vertexData(vertexData)
    , displayData(std::move(displayData))
    {
    }

    DisplayBlock(DisplayType type,
                 const mat4* mat,
                 const VertexData* vertexData,
                 const IndexData* indexData,
                 std::vector<const DisplayData*>& displayData)
    : type(type)
    , hide(false)
    , mat(mat)
    , vertexData(vertexData)
    , indexData(indexData)
    , displayData(std::move(displayData))
    {
    }

    DisplayBlock(DisplayType type,
                 const mat4* mat,
                 const VertexData* vertexData,
                 const DisplayData* data)
    : type(type)
    , hide(false)
    , mat(mat)
    , vertexData(vertexData)
    {
        displayData.push_back(data);
    }

    DisplayBlock(DisplayType type,
                 const mat4* mat,
                 const VertexData* vertexData,
                 const IndexData* indexData,
                 const DisplayData* data)
    : type(type)
    , hide(false)
    , mat(mat)
    , vertexData(vertexData)
    , indexData(indexData)
    {
        displayData.push_back(data);
    }
};

using DisplayList = std::vector<const DisplayBlock*>;
using InstancedList = std::vector<const InstanceData*>;

} //namespace render