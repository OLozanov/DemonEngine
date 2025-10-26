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

enum class LayerOrientation : uint8_t
{
    Normal,
    Rot90,
    Rot180,
    Rot270
};

struct SurfaceLayer
{
    LayerOrientation orientation;
    ResourcePtr<Material> material;
    Render::VertexArray<float> vertexBuffer;

    SurfaceLayer(Material* mat, size_t size)
    : material(mat)
    , orientation(LayerOrientation::Normal)
    {
        vertexBuffer.resize(size);

        for (size_t i = 0; i < size; i++) vertexBuffer[i] = 0;
    }

    SurfaceLayer(Material* mat, LayerOrientation orientation, const std::vector<float>& layerMask)
    : material(mat)
    , orientation(orientation)
    , vertexBuffer(layerMask)
    {
    }
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

    EditSurface(Block* block, BlockPolygon* polygon, size_t size);
    EditSurface(BlockPolygon* polygon, size_t size,
                const std::vector<TexturedVertex>& vertices,
                std::vector<vec3>& normals);

    void link(Block* block, BlockPolygon* polygon);

    size_t size() const { return m_xsize; }

    void setMaterial(Material* material) { m_material.reset(material); }
    const Material* material() const { return m_material.get(); }

    const Block& owner() const { return *m_owner; }

    const vec3& normal(size_t i, size_t k) const { return m_normals[k * m_xsize + i]; }
    vec3& normal(size_t i, size_t k) { return m_normals[k * m_xsize + i]; }

    void setVertex(size_t i, size_t k, const vec3& vert) { m_tempVertices[k * m_xsize + i] = vert; }
    void setNormal(size_t i, size_t k, const vec3& norm) { m_tempNormals[k * m_xsize + i] = norm; }

    void updateTempBuffers();
    void applyChanges();

    void applyTransform(const mat4& mat);
    void scale(const vec3& scale);

    void addLayer(Material* material);
    void addLayer(Material* material, LayerOrientation orientation, const std::vector<float>& layerMask);
    void deleteLayer(size_t n);
    const std::vector<SurfaceLayer>& layers() { return m_layers; }

    void addDetails(size_t layer, const std::string& model, const std::string& material, float density);
    void removeDetails(size_t n);
    void clearDetails();
    const std::vector<SurfaceLayerDetails>& details() { return m_layerDetails; }

    void displace(const vec3& point, float power, float radius);
    void paintLayer(const vec3& point, float radius, size_t layer);

    void collectVertices(const vec3& center, float radius, std::vector<SurfaceVertexLink>& vlist);
    void convolve(const vec3& center, float radius, float& value, vec3& norm, float& num) const;

    void writeLayers(FILE* file) const;
    void writeLayerDetails(FILE* file) const;
    void write(FILE* file) const;
    void writeGameInfo(FILE* file) const;

    void setSurfaceGraph(SurfaceGraphPtr& surfaceGraph) { m_surfaceGraph = surfaceGraph; }
    SurfaceGraph* surfaceGraph() { return m_surfaceGraph.get(); }

    void buildVertices();

    const Vertex& tsVertex(size_t i, size_t k) const { return m_vertices[k * m_xsize + i]; }
    Vertex& tsVertex(size_t i, size_t k) { return m_vertices[k * m_xsize + i]; }

    void displayLayers(Render::CommandList& commandList) const;

private:
    void tesselate(const Block* block, const BlockPolygon* poly);
    void initNormals(const Block* block, const BlockPolygon* poly);

    Block* m_owner;
    BlockPolygon* m_polygon;

    ResourcePtr<Material> m_material;

    std::vector<vec3> m_normals;

    std::vector<vec3> m_tempVertices;
    std::vector<vec3> m_tempNormals;
    
    std::vector<Vertex> m_vertices;

    std::vector<SurfaceLayer> m_layers;
    std::vector<SurfaceLayerDetails> m_layerDetails;

    SurfaceGraphPtr m_surfaceGraph;
};