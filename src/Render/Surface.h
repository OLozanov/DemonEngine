#pragma once

#include "Render/DisplayObject.h"
#include "Render/Render.h"

#include "Resources/Resources.h"

#include <vector>

namespace Render
{

struct SurfaceLayer
{
    uint8_t orientation;
    Material* material;
};

struct SurfaceLayerDetails
{
    uint8_t layer;
    std::string model;
    std::string detail;
    float density;
};

struct DetailInstanceData
{
    ModelPtr model;
    MaterialPtr material;
    VertexBuffer instanceBuffer;
};

class Surface : public DisplayObject
{
protected:
    DisplayData m_displayElement;

    ResourcePtr<Material> m_material;
    VertexBuffer m_vertexBuffer;
    IndexBuffer m_indexBuffer;

    Surface() {}

public:
    Surface(const vec3& pos,
            Material* mat,
            long xsize,
            long ysize,
            const std::vector<Vertex>& vertices);

    Surface(const vec3& pos,
            Material* mat,
            long res,
            const std::vector<Vertex>& vertices);

    virtual ~Surface() {}

    const VertexBuffer& vertexBuffer() const { return m_vertexBuffer; }
    const IndexBuffer& indexBuffer() const { return m_indexBuffer; }

protected:

    static bool isDegenerateTriangle(const vec3& a, const vec3& b, const vec3& c);

    void calculateBBox(const std::vector<Vertex>& vertices);
};

class LayeredSurface : public Surface
{
    static constexpr size_t MaxLayers = 8;

    struct Parameters
    {
        uint32_t baseMaterial;
        uint32_t layernum;
        uint32_t width;
        uint32_t height;
        uint32_t layers[MaxLayers * 2];
    };

    std::vector<MaterialPtr> m_layers;

    Parameters m_parameters;
    Buffer<float> m_maskBuffer;

    std::vector<DetailInstanceData> m_detailInstanceData;

public:
    LayeredSurface(const vec3& pos,
                   Material* mat,
                   long xsize,
                   long ysize,
                   const std::vector<Vertex>& vertices,
                   const std::vector<SurfaceLayer>& layers,
                   const std::vector<float>& layerMask,
                   const std::vector<Render::SurfaceLayerDetails>& layerDetails);

private:
    void generateDetailInstances(const SurfaceLayerDetails& layerDetails, 
                                 uint32_t xsize, uint32_t ysize, 
                                 const std::vector<Vertex>& verts,
                                 const std::vector<SurfaceLayer>& layers,
                                 const std::vector<float>& layerMask,
                                 vec3& maxsize);
};

} //namespace Render