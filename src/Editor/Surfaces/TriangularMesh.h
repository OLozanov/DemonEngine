#pragma once

#include "math/math3d.h"
#include "Render/Render.h"
#include "Resources/Resources.h"

class Block;
class BlockPolygon;
class Material;

using TexturedVertex = Render::TexturedVertex;

class TriangularMesh
{
public:
    TriangularMesh(size_t res);

    void resize(size_t res);

    const BBox& bbox() const { return m_bbox; }

    const TexturedVertex& vertex(size_t i) const { return m_vertexBuffer[i]; }
    TexturedVertex& vertex(size_t i) { return m_vertexBuffer[i]; }

    void updateBBox();

    size_t res() const { return m_res; }
    size_t size() const { return m_res * (m_res + 1) / 2; }

    void moveTexCoordS(float val);
    void moveTexCoordT(float val);
    void scaleTexCoordS(float val);
    void scaleTexCoordT(float val);
    void rotateTexCoord(float ang);

    bool pick(const vec3& origin, const vec3& ray, float& dist) const;

    void display(Render::CommandList& commandList) const;

protected:
    TriangularMesh() = default;

    void initIndices();

protected:
    size_t m_res;

    size_t m_indexNum;

    Render::VertexArray<TexturedVertex> m_vertexBuffer;
    Render::IndexBuffer m_indexBuffer;

    BBox m_bbox;
};
