#pragma once

#include "math/math3d.h"
#include "Render/Render.h"
#include "Resources/Resources.h"

using TexturedVertex = Render::TexturedVertex;

class SurfaceMesh
{
public:
    SurfaceMesh(size_t xsize, size_t ysize);

    void resize(size_t xsize, size_t ysize);

    const BBox& bbox() const { return m_bbox; }

    const TexturedVertex& vertex(size_t i) const { return m_vertices[i]; }
    TexturedVertex& vertex(size_t i) { return m_vertices[i]; }

    const TexturedVertex& vertex(size_t i, size_t k) const { return m_vertices[k * m_xsize + i]; }
    TexturedVertex& vertex(size_t i, size_t k) { return m_vertices[k * m_xsize + i]; }

    void updateBBox();
    void flushVertices();

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

    std::vector<TexturedVertex> m_vertices;

    Render::VertexArray<TexturedVertex> m_vertexBuffer;
    Render::IndexBuffer m_indexBuffer;

    BBox m_bbox;
};