#pragma once

#include "Geometry/Geometry.h"

#include "math/math3d.h"
#include <vector>
#include <memory>

class EditSurface;

struct SurfaceFace
{
    EditSurface* surface;

    size_t vertices[4];
    size_t edges[4];

    SurfaceFace(EditSurface* surf, size_t v[], size_t e[])
    : surface(surf)
    {
        for (int i = 0; i < 4; i++)
        {
            vertices[i] = v[i];
            edges[i] = e[i];
        }
    }
};

struct SurfaceEdge
{
    size_t a;
    size_t b;

    std::vector<size_t> faces; // maybe limit to only 2 ???

    SurfaceEdge(size_t a, size_t b)
    : a(a)
    , b(b)
    {
    }
};

struct SurfaceVertex
{
    vec3 pos;
    std::vector<size_t> edges;
    std::vector<size_t> faces;

    SurfaceVertex(const vec3& v) : pos(v) {}
};

class SurfaceGraph
{
public:
    SurfaceGraph(size_t id, const std::vector<EditSurface*>& surfaces);

    size_t id() { return m_id; }

    void smooth(const vec3& center, float power, float radius);
    void buildGeometry();

private:
    bool findVertex(const vec3& vert, size_t& ind);
    bool findEdge(size_t a, size_t b, size_t& ind);
    bool findEdgeNeighbor(size_t face, size_t a, size_t b, size_t& ind);
    std::vector<size_t> findVertexEdges(size_t v);

    void fillEdgeVertexPtr(SurfaceFace& face, std::vector<Vertex*>& verts, size_t a, size_t b);
    void blendEdgeTangentSpace(size_t e);
    void blendVertexTangentSpace(size_t v);

    void addConjugateEdgeVertex(size_t faceid, size_t a, size_t b, uint16_t i, vec3& pos, vec3& norm);
    void addConjugateVertices(size_t faceid, size_t a, size_t b, size_t c, vec3& pos, vec3& norm, int& n);
    void addConjugateVertices(size_t v, vec3& pos, vec3& norm, int& n);

    void laplacian(size_t face, uint16_t i, uint16_t k, vec3& pos, vec3& norm);

    void smoothPass(const vec3& center, float vertFactor, float normFactor, float radius);

private:
    size_t m_id;

    std::vector<SurfaceVertex> m_vertices;
    std::vector<SurfaceEdge> m_edges;
    std::vector<SurfaceFace> m_faces;
};

using SurfaceGraphPtr = std::shared_ptr<SurfaceGraph>;