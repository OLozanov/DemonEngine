#pragma once

#include "SurfaceMesh.h"
#include "Geometry/Geometry.h"

#include "Surfaces/SurfaceGraph.h"

class Block;
class BlockPolygon;

struct SurfaceVertexLink
{
    const vec3& pos;
    vec3& vert;
    vec3& norm;
};

enum class LayerType : uint8_t
{
    TCoord,
    Triplanar
};

struct SurfaceLayer
{
    LayerType type;
    ResourcePtr<Material> material;

    std::vector<float> mask;

    SurfaceLayer(Material* mat, size_t size)
    : material(mat)
    , type(LayerType::TCoord)
    , mask(size * size)
    {
    }

    SurfaceLayer(Material* mat, LayerType type, size_t size)
    : material(mat)
    , type(type)
    , mask(size * size)
    {
    }

    SurfaceLayer(SurfaceLayer&& ptr) noexcept = default;
    SurfaceLayer& operator=(SurfaceLayer&& ptr) noexcept = default;
};

struct SurfaceLayerDetails
{
    size_t layer;
    std::string model;
    std::string material;
    float density;

    SurfaceLayerDetails(size_t layer, const std::string& model, const std::string& material, float density)
    : layer(layer)
    , model(model)
    , material(material)
    , density(density)
    {
    }
};

class EditSurface : public SurfaceMesh
{
public:
    using MaskBuffer = Render::StreamBuffer<float>;

public:
    EditSurface(Block* block, BlockPolygon* polygon, size_t size);
    EditSurface(BlockPolygon* polygon, size_t size,
                const std::vector<TexturedVertex>& vertices,
                std::vector<vec3>& normals);

    void link(Block* block, BlockPolygon* polygon);

    size_t size() const { return m_xsize; }

    void setMaterial(Material* material) { m_material.reset(material); }
    const Material* material() const { return m_material.get(); }

    void setMaterial(Material* material, size_t layer);

    void enableTesselation(bool enable) { m_tesselation = enable; }
    bool tesselationEnabled() const { return m_tesselation; }

    const Block& owner() const { return *m_owner; }

    const vec3& normal(size_t i, size_t k) const { return m_normals[k * m_xsize + i]; }
    vec3& normal(size_t i, size_t k) { return m_normals[k * m_xsize + i]; }

    void setVertex(size_t i, size_t k, const vec3& vert) { m_tempVertices[k * m_xsize + i] = vert; }
    void setNormal(size_t i, size_t k, const vec3& norm) { m_tempNormals[k * m_xsize + i] = norm; }

    void updateTempBuffers();
    void applyChanges();

    void applyTransform(const mat4& mat);
    void scale(const vec3& scale);

    void setLayerMask(const std::vector<float>& layerMask);

    void addLayer(Material* material);
    void addLayer(Material* material, LayerType type);
    void deleteLayer(size_t n);
    const std::vector<SurfaceLayer>& layers() const { return m_layers; }
    SurfaceLayer& layer(size_t n) { return m_layers[n]; }
    const SurfaceLayer& layer(size_t n) const { return m_layers[n]; }

    void setLayerType(LayerType type);
    void setLayerType(LayerType type, size_t layer);

    LayerType baseLayerType() const { return m_baseLayer; }
    
    void swapLayers(size_t l1, size_t l2);
    void moveLayerUp(size_t layer);
    void moveLayerDown(size_t layer);

    size_t layerNum() const { return m_layers.size(); }

    const MaskBuffer& maskBuffer() const { return m_maskBuffer; }
    const Render::StreamBuffer<uint32_t>& layersBuffer() const { return m_layersBuffer; }

    void addDetails(size_t layer, const std::string& model, const std::string& material, float density);
    void removeDetails(size_t n);
    void clearDetails();
    const std::vector<SurfaceLayerDetails>& details() { return m_layerDetails; }

    void displace(const vec3& point, float power, float radius);
    void paintLayer(const vec3& point, float radius, size_t layer);
    void eraseLayer(const vec3& point, float radius, size_t layer);

    void collectVertices(const vec3& center, float radius, std::vector<SurfaceVertexLink>& vlist);
    void convolve(const vec3& center, float radius, float& value, vec3& norm, float& num) const;

    void writeLayers(FILE* file) const;
    void writeLayerDetails(FILE* file) const;
    void write(FILE* file) const;
    void writeGameInfo(FILE* file) const;

    void setSurfaceGraph(SurfaceGraphPtr& surfaceGraph) { m_surfaceGraph = surfaceGraph; }
    SurfaceGraph* surfaceGraph() { return m_surfaceGraph.get(); }

    void buildGeometry();

    const Vertex& tsVertex(size_t i, size_t k) const { return m_geometry[k * m_xsize + i]; }
    Vertex& tsVertex(size_t i, size_t k) { return m_geometry[k * m_xsize + i]; }

private:
    void tesselate(const Block* block, const BlockPolygon* poly);
    void initNormals(const Block* block, const BlockPolygon* poly);

    Block* m_owner;
    BlockPolygon* m_polygon;

    bool m_tesselation;

    LayerType m_baseLayer;
    ResourcePtr<Material> m_material;

    std::vector<vec3> m_normals;

    std::vector<vec3> m_tempVertices;
    std::vector<vec3> m_tempNormals;
    
    std::vector<Vertex> m_geometry;

    std::vector<SurfaceLayer> m_layers;
    std::vector<SurfaceLayerDetails> m_layerDetails;

    MaskBuffer m_maskBuffer;
    Render::StreamBuffer<uint32_t> m_layersBuffer;

    SurfaceGraphPtr m_surfaceGraph;
};