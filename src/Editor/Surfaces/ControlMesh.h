#pragma once

#include "math/math3d.h"
#include "Render/Render.h"

class ControlMesh
{
public:
    ControlMesh(size_t xsize, size_t ysize);
    ControlMesh(size_t size);

    const vec3& vertex(size_t i) const { return m_vertexBuffer[i]; }
    vec3& vertex(size_t i) { return m_vertexBuffer[i]; }

    const vec3& vertex(size_t i, size_t k) const { return m_vertexBuffer[k * m_xsize + i]; }
    vec3& vertex(size_t i, size_t k) { return m_vertexBuffer[k * m_xsize + i]; }

    void display(Render::CommandList& commandList) const;
    void displayVertices(Render::CommandList& commandList) const;

private:
    void initIndices();
    void initTriangularIndices();

private:
    const size_t m_xsize;
    const size_t m_ysize;

    size_t m_indexNum;

    Render::VertexArray<vec3> m_vertexBuffer;
    Render::IndexBuffer m_indexBuffer;
};