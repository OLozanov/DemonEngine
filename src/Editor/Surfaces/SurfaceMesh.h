#pragma once

#include "math/math3d.h"
#include "Render/Render.h"
#include "Resources/Resources.h"

using TexturedVertex = Render::TexturedVertex;

struct NormVertex
{
    vec3 position;
    vec2 tcoord;
    vec3 normal;
};

class SurfaceMesh
{
public:
    SurfaceMesh(size_t xsize, size_t ysize);

    void resize(size_t xsize, size_t ysize);

    const BBox& bbox() const { return m_bbox; }

    const NormVertex& vertex(size_t i) const { return m_vertices[i]; }
    NormVertex& vertex(size_t i) { return m_vertices[i]; }

    const NormVertex& vertex(size_t i, size_t k) const { return m_vertices[k * m_xsize + i]; }
    NormVertex& vertex(size_t i, size_t k) { return m_vertices[k * m_xsize + i]; }

    size_t index(size_t i, size_t k) { return k * m_xsize + i; }

    void calculateNormals();

    void updateBBox();
    void updateNormals();
    void flushVertices();
    void flushNormals();

    size_t xsize() const { return m_xsize; }
    size_t ysize() const { return m_ysize; }

    void moveTexCoordS(float val);
    void moveTexCoordT(float val);
    void scaleTexCoordS(float val);
    void scaleTexCoordT(float val);
    void rotateTexCoord(float ang);

    bool pick(const vec3& origin, const vec3& ray, float& dist) const;

    void display(Render::CommandList& commandList) const;

protected:
    SurfaceMesh() = default;

    void initIndices();

protected:

    size_t m_xsize;
    size_t m_ysize;

    size_t m_indexNum;

    std::vector<NormVertex> m_vertices;

    Render::VertexArray<NormVertex> m_vertexBuffer;
    Render::IndexBuffer m_indexBuffer;

    BBox m_bbox;
};