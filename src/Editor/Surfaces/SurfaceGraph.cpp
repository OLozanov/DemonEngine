#include "SurfaceGraph.h"
#include "Surfaces/EditSurface.h"
#include "Geometry/Block.h"

SurfaceGraph::SurfaceGraph(size_t id, const std::vector<EditSurface*>& surfaces)
: m_id(id)
{
    for (EditSurface* surface : surfaces)
    {
        size_t size = surface->xsize();

        const vec3& pos = surface->owner().pos();

        vec3 verts[4] = { surface->vertex(0, 0).position + pos,
                          surface->vertex(size-1, 0).position + pos,
                          surface->vertex(size-1, size-1).position + pos,
                          surface->vertex(0, size-1).position + pos};

        size_t vid[4];

        for (int i = 0; i < 4; i++)
        {
            size_t ind;

            if (findVertex(verts[i], ind))
            {
                vid[i] = ind;
            }
            else
            {
                vid[i] = m_vertices.size();
                m_vertices.push_back(verts[i]);
            }

            m_vertices[vid[i]].faces.push_back(m_faces.size());
        }

        size_t edges[4];

        for (int i = 0; i < 4; i++)
        {
            int k = i == 3 ? 0 : i + 1;

            size_t ind;
            size_t a = vid[i];
            size_t b = vid[k];

            if (findEdge(a, b, ind))
            {
                edges[i] = ind;
            }
            else
            {
                edges[i] = m_edges.size();
                m_edges.emplace_back(a, b);

                m_vertices[a].edges.emplace_back(edges[i]);
                m_vertices[b].edges.emplace_back(edges[i]);
            }

            m_edges[edges[i]].faces.push_back(m_faces.size());
        }

        m_faces.emplace_back(surface, vid, edges);
    }
}

bool SurfaceGraph::findVertex(const vec3& vert, size_t& ind)
{
    for (size_t i = 0; i < m_vertices.size(); i++)
    {
        if ((vert - m_vertices[i].pos).length() < math::eps)
        {
            ind = i;
            return true;
        }
    }

    return false;
}

bool SurfaceGraph::findEdge(size_t a, size_t b, size_t& ind)
{
    for (size_t i = 0; i < m_edges.size(); i++)
    {
        if ((m_edges[i].a == a && m_edges[i].b == b) ||
            (m_edges[i].a == b && m_edges[i].b == a))
        {
            ind = i;
            return true;
        }
    }

    return false;
}

bool SurfaceGraph::findEdgeNeighbor(size_t face, size_t a, size_t b, size_t& ind)
{
    size_t edge;
    findEdge(a, b, edge);

    for (size_t i = 0; i < m_faces.size(); i++)
    {
        if (i != face)
        {
            for (int e = 0; e < 4; e++)
            {
                if (m_faces[i].edges[e] == edge)
                {
                    ind = i;
                    return true;
                }
            }
        }
    }

    return false;
}

std::vector<size_t> SurfaceGraph::findVertexEdges(size_t v)
{
    std::vector<size_t> edges;

    for (size_t i = 0; i < m_edges.size(); i++)
    {
        if (m_edges[i].a == v || m_edges[i].b == v) edges.push_back(i);
    }

    return edges;
}

void SurfaceGraph::fillEdgeVertexPtr(SurfaceFace& face, std::vector<Vertex*>& verts, size_t a, size_t b)
{
    EditSurface* surf = face.surface;
    uint16_t sz = surf->size();

    verts.resize(sz);

    if (face.vertices[0] == a && face.vertices[1] == b)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(i, 0);
    }

    if (face.vertices[0] == b && face.vertices[1] == a)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(sz - i - 1, 0);
    }

    if (face.vertices[1] == a && face.vertices[2] == b)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(sz - 1, i);
    }

    if (face.vertices[1] == b && face.vertices[2] == a)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(sz - 1, sz - i - 1);
    }

    if (face.vertices[3] == a && face.vertices[2] == b)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(i, sz - 1);
    }

    if (face.vertices[3] == b && face.vertices[2] == a)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(sz - i - 1, sz - 1);
    }

    if (face.vertices[0] == a && face.vertices[3] == b)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(0, i);
    }

    if (face.vertices[0] == b && face.vertices[3] == a)
    {
        for (size_t i = 0; i < sz; i++) verts[i] = &surf->tsVertex(0, sz - i - 1);
    }
}

void SurfaceGraph::blendEdgeTangentSpace(size_t e)
{
    const SurfaceEdge& edge = m_edges[e];

    if (edge.faces.size() < 2) return;

    SurfaceFace& face1 = m_faces[edge.faces[0]];
    SurfaceFace& face2 = m_faces[edge.faces[1]];

    std::vector<Vertex*> vptr1;
    std::vector<Vertex*> vptr2;

    fillEdgeVertexPtr(face1, vptr1, edge.a, edge.b);
    fillEdgeVertexPtr(face2, vptr2, edge.a, edge.b);

    for (size_t i = 0; i < vptr1.size(); i++)
    {
        vec3 normal = (vptr1[i]->normal + vptr2[i]->normal) * 0.5f;
        vec3 tangent = (vptr1[i]->tangent + vptr2[i]->tangent) * 0.5f;
        vec3 binormal = (vptr1[i]->binormal + vptr2[i]->binormal) * 0.5f;

        normal.normalize();
        tangent.normalize();
        binormal.normalize();

        vptr1[i]->normal = normal;
        vptr1[i]->tangent = tangent;
        vptr1[i]->binormal = binormal;

        vptr2[i]->normal = normal;
        vptr2[i]->tangent = tangent;
        vptr2[i]->binormal = binormal;
    }
}

void SurfaceGraph::blendVertexTangentSpace(size_t v)
{
    const SurfaceVertex& vert = m_vertices[v];

    if (vert.faces.size() <= 1) return;

    vec3 normal = {};
    vec3 tangent = {};
    vec3 binormal = {};

    std::vector<Vertex*> tsverts(vert.faces.size());

    for (int f = 0; f < vert.faces.size(); f++)
    {
        SurfaceFace& face = m_faces[vert.faces[f]];
        EditSurface* surf = face.surface;
    
        uint16_t max = surf->size() - 1;

        size_t i, k;

        if (v == face.vertices[0]) { i = 0; k = 0; }
        if (v == face.vertices[1]) { i = max; k = 0; }
        if (v == face.vertices[2]) { i = max; k = max; }
        if (v == face.vertices[3]) { i = 0; k = max; }

        Vertex& tsvert = surf->tsVertex(i, k);

        normal += tsvert.normal;
        tangent += tsvert.tangent;
        binormal += tsvert.binormal;

        tsverts[f] = &tsvert;
    }

    normal.normalize();
    tangent.normalize();
    binormal.normalize();

    for (int v = 0; v < tsverts.size(); v++)
    {
        tsverts[v]->normal = normal;
        tsverts[v]->tangent = tangent;
        tsverts[v]->binormal = binormal;
    }
}

void SurfaceGraph::addConjugateEdgeVertex(size_t faceid, size_t a, size_t b, uint16_t i, vec3& pos, vec3& norm)
{
    SurfaceFace& face = m_faces[faceid];
    const EditSurface* surf = face.surface;

    uint16_t max = surf->size() - 1;

    if (face.vertices[0] == a && face.vertices[1] == b)
    {
        pos += surf->vertex(i, 1).position;
        norm += surf->normal(i, 1);
    }

    if (face.vertices[0] == b && face.vertices[1] == a)
    {
        pos += surf->vertex(max - i, 1).position;
        norm += surf->normal(max - i, 1);
    }

    if (face.vertices[1] == a && face.vertices[2] == b)
    {
        pos += surf->vertex(max - 1, i).position;
        norm += surf->normal(max - 1, i);
    }

    if (face.vertices[1] == b && face.vertices[2] == a)
    {
        pos += surf->vertex(max - 1, max - i).position;
        norm += surf->normal(max - 1, max - i);
    }

    if (face.vertices[3] == a && face.vertices[2] == b)
    {
        pos += surf->vertex(i, max - 1).position;
        norm += surf->normal(i, max - 1);
    }

    if (face.vertices[3] == b && face.vertices[2] == a)
    {
        pos += surf->vertex(max - i, max - 1).position;
        norm += surf->normal(max - i, max - 1);
    }

    if (face.vertices[0] == a && face.vertices[3] == b)
    {
        pos += surf->vertex(1, i).position;
        norm += surf->normal(1, i);
    }

    if (face.vertices[0] == b && face.vertices[3] == a)
    {
        pos += surf->vertex(1, max - i).position;
        norm += surf->normal(1, max - i);
    }

    pos += surf->owner().pos();
}

void SurfaceGraph::addConjugateVertices(size_t faceid,
                                        size_t a, size_t b, size_t c,
                                        vec3& pos, vec3& norm, int& n)
{
    size_t e = c;

    while (true)
    {
        size_t id;

        if (!findEdgeNeighbor(faceid, b, e, id)) break;

        SurfaceFace& face = m_faces[id];
        EditSurface* surf = face.surface;

        faceid = id;

        uint16_t max = surf->size() - 1;

        for (int i = 0; i < 4; i++)
        {
            size_t vert = face.vertices[i];

            if (vert == b)
            {
                int prev = i == 0 ? 3 : i - 1;
                int next = i == 3 ? 0 : i + 1;

                int k;

                if (e == face.vertices[prev]) k = next;
                if (e == face.vertices[next]) k = prev;

                e = face.vertices[k];

                if (e == a) return;

                size_t l, m;

                if (i == 0 && k == 1) { l = 1; m = 0; }
                if (i == 1 && k == 0) { l = max - 1; m = 0; }

                if (i == 1 && k == 2) { l = max; m = 1; }
                if (i == 2 && k == 1) { l = max; m = max - 1; }

                if (i == 2 && k == 3) { l = max - 1; m = max; }
                if (i == 3 && k == 2) { l = 1; m = max; }

                if (i == 3 && k == 0) { l = 0; m = max - 1; }
                if (i == 0 && k == 3) { l = 0; m = 1; }

                pos += surf->vertex(l, m).position + surf->owner().pos();
                norm += surf->normal(l, m);

                n++;
              
                break;
            }
        }
    }
}

void SurfaceGraph::addConjugateVertices(size_t v, vec3& pos, vec3& norm, int& n)
{
    if (m_vertices[v].faces.size() == 1) return;

    vec3 ipos = {};
    vec3 inorm = {};
    int inum = 0;

    bool loop = true;

    for (size_t eid : m_vertices[v].edges)
    {
        const SurfaceEdge& edge = m_edges[eid];

        size_t v2 = edge.a == v ? edge.b : edge.a;

        size_t faceid = edge.faces[0];

        SurfaceFace& face = m_faces[faceid];
        EditSurface* surf = face.surface;

        size_t facenum = edge.faces.size();
        uint16_t max = surf->size() - 1;

        for (int i = 0; i < 4; i++)
        {
            size_t vert = face.vertices[i];

            if (vert == v)
            {
                int prev = i == 0 ? 3 : i - 1;
                int next = i == 3 ? 0 : i + 1;

                int k;

                if (v2 == face.vertices[prev]) k = prev;
                if (v2 == face.vertices[next]) k = next;

                size_t l, m;

                if (i == 0 && k == 1) { l = 1; m = 0; }
                if (i == 1 && k == 0) { l = max - 1; m = 0; }

                if (i == 1 && k == 2) { l = max; m = 1; }
                if (i == 2 && k == 1) { l = max; m = max - 1; }

                if (i == 2 && k == 3) { l = max - 1; m = max; }
                if (i == 3 && k == 2) { l = 1; m = max; }

                if (i == 3 && k == 0) { l = 0; m = max - 1; }
                if (i == 0 && k == 3) { l = 0; m = 1; }

                if (facenum == 1)
                {
                    pos += surf->vertex(l, m).position + surf->owner().pos();
                    norm += surf->normal(l, m);

                    n++;

                    loop = false;
                }
                else
                {
                    ipos += surf->vertex(l, m).position + surf->owner().pos();
                    inorm += surf->normal(l, m);

                    inum++;
                }

                break;
            }
        }
    }

    if (loop)
    {
        pos += ipos;
        norm += inorm;

        n += inum;
    }
}

void SurfaceGraph::laplacian(size_t faceid, uint16_t i, uint16_t k, vec3& pos, vec3& norm)
{
    const SurfaceFace& face = m_faces[faceid];

    const EditSurface* surf = face.surface;
    const vec3& ownerPos = surf->owner().pos();

    uint16_t max = surf->size() - 1;

    if ((i > 0 && i < max) && (k > 0 && k < max))
    {
        pos = surf->vertex(i, k).position +
              surf->vertex(i + 1, k).position +
              surf->vertex(i - 1, k).position +
              surf->vertex(i, k + 1).position +
              surf->vertex(i, k - 1).position +
              surf->owner().pos() * 5;

        pos *= 0.2;

        norm = surf->normal(i, k) +
               surf->normal(i + 1, k) +
               surf->normal(i - 1, k) +
               surf->normal(i, k + 1) +
               surf->normal(i, k - 1);

        norm *= 0.2;

        return;
    }

    // corners
    if (i == 0 && k == 0)
    {
        pos = surf->vertex(i, k).position + surf->owner().pos();
        norm = surf->normal(i, k);
        int n = 1;

        size_t a = face.vertices[3];
        size_t b = face.vertices[0];
        size_t c = face.vertices[1];

        addConjugateVertices(b, pos, norm, n);

        pos *= 1.0f / n;
        norm *= 1.0f / n;

        return;
    }

    if (i == max && k == 0)
    {
        pos = surf->vertex(i, k).position + surf->owner().pos();
        norm = surf->normal(i, k);
        int n = 1;

        size_t a = face.vertices[0];
        size_t b = face.vertices[1];
        size_t c = face.vertices[2];

        addConjugateVertices(b, pos, norm, n);

        pos *= 1.0f / n;
        norm *= 1.0f / n;

        return;
    }

    if (i == max && k == max)
    {
        pos = surf->vertex(i, k).position + surf->owner().pos();
        norm = surf->normal(i, k);
        int n = 1;

        size_t a = face.vertices[1];
        size_t b = face.vertices[2];
        size_t c = face.vertices[3];

        addConjugateVertices(b, pos, norm, n);

        pos *= 1.0f / n;
        norm *= 1.0f / n;

        return;
    }

    if (i == 0 && k == max)
    {
        pos = surf->vertex(i, k).position + surf->owner().pos();
        norm = surf->normal(i, k);
        int n = 1;

        size_t a = face.vertices[2];
        size_t b = face.vertices[3];
        size_t c = face.vertices[0];

        addConjugateVertices(b, pos, norm, n);

        pos *= 1.0f / n;
        norm *= 1.0f / n;

        return;
    }

    // edges
    if (i == 0)
    {
        pos = surf->vertex(i, k).position +
              surf->vertex(i, k + 1).position +
              surf->vertex(i, k - 1).position +
              surf->owner().pos() * 3;

        norm = surf->normal(i, k) +
               surf->normal(i, k + 1) +
               surf->normal(i, k - 1);

        size_t id;

        size_t a = face.vertices[0];
        size_t b = face.vertices[3];

        if (findEdgeNeighbor(faceid, a, b, id))
        {
            addConjugateEdgeVertex(id, a, b, k, pos, norm);

            pos += surf->vertex(i + 1, k).position + surf->owner().pos();
            norm += surf->normal(i + 1, k);

            pos *= 0.2;
            norm *= 0.2;
        }
        else
        {
            pos *= 1.0 / 3.0;
            norm *= 1.0 / 3.0;
        }
    }

    if (i == max)
    {
        pos = surf->vertex(i, k).position +
              surf->vertex(i, k + 1).position +
              surf->vertex(i, k - 1).position +
              surf->owner().pos() * 3;

        norm = surf->normal(i, k) +
               surf->normal(i, k + 1) +
               surf->normal(i, k - 1);

        size_t id;

        size_t a = face.vertices[1];
        size_t b = face.vertices[2];

        if (findEdgeNeighbor(faceid, a, b, id))
        {
            addConjugateEdgeVertex(id, a, b, k, pos, norm);

            pos += surf->vertex(i - 1, k).position + surf->owner().pos();
            norm += surf->normal(i - 1, k);

            pos *= 0.2;
            norm *= 0.2;
        }
        else
        {
            pos *= 1.0 / 3.0;
            norm *= 1.0 / 3.0;
        }
    }

    if (k == 0)
    {
        pos = surf->vertex(i, k).position +
              surf->vertex(i + 1, k).position +
              surf->vertex(i - 1, k).position +
              surf->owner().pos() * 3;

        norm = surf->normal(i, k) +
               surf->normal(i + 1, k) +
               surf->normal(i - 1, k);

        size_t id;

        size_t a = face.vertices[0];
        size_t b = face.vertices[1];

        if (findEdgeNeighbor(faceid, a, b, id))
        {
            addConjugateEdgeVertex(id, a, b, i, pos, norm);

            pos += surf->vertex(i, k + 1).position + surf->owner().pos();
            norm += surf->normal(i, k + 1);

            pos *= 0.2;
            norm *= 0.2;
        }
        else
        {
            pos *= 1.0 / 3.0;
            norm *= 1.0 / 3.0;
        }
    }

    if (k == max)
    {
        pos = surf->vertex(i, k).position +
              surf->vertex(i + 1, k).position +
              surf->vertex(i - 1, k).position +
              surf->owner().pos() * 3;

        norm = surf->normal(i, k) +
               surf->normal(i + 1, k) +
               surf->normal(i - 1, k);

        size_t id;

        size_t a = face.vertices[3];
        size_t b = face.vertices[2];

        if (findEdgeNeighbor(faceid, a, b, id))
        {
            addConjugateEdgeVertex(id, a, b, i, pos, norm);

            pos += surf->vertex(i, k - 1).position + surf->owner().pos();
            norm += surf->normal(i, k - 1);

            pos *= 0.2;
            norm *= 0.2;
        }
        else
        {
            pos *= 1.0 / 3.0;
            norm *= 1.0 / 3.0;
        }
    }
}

void SurfaceGraph::smoothPass(const vec3& center, float vertFactor, float normFactor, float radius)
{
    BBox bbox = { vec3(-radius, -radius, -radius), vec3(radius, radius, radius) };

    for (SurfaceFace& face : m_faces) face.surface->updateTempBuffers();

    for (size_t f = 0; f < m_faces.size(); f++)
    {
        const SurfaceFace& face = m_faces[f];
        size_t size = face.surface->size();

        const vec3& ownerPos = face.surface->owner().pos();

        if (!BBoxIntersect(center, bbox, ownerPos, face.surface->bbox())) continue;
        
        for (size_t i = 0; i < size; i++)
        {
            for (size_t k = 0; k < size; k++)
            {
                vec3 vert = face.surface->vertex(i, k).position;
                vec3 vnormal = face.surface->normal(i, k);
                float dist = (center - (vert + ownerPos)).length();

                if (dist > radius) continue;

                vec3 pos;
                vec3 norm;

                laplacian(f, i, k, pos, norm);

                pos -= ownerPos;
                vert += (pos - vert) * vertFactor;
                vnormal += (norm - vnormal) * normFactor;
                vnormal.normalize();

                face.surface->setVertex(i, k, vert);
                face.surface->setNormal(i, k, vnormal);
            }
        }
    }

    for (SurfaceFace& face : m_faces)
    {
        face.surface->updateBBox();
        face.surface->applyChanges();
    }
}

void SurfaceGraph::smooth(const vec3& center, float power, float radius)
{
    float vertFactor = 0.8 * power / 100.0;
    float normFactor = 0.5 + 0.25 * power / 100.0;

    smoothPass(center, vertFactor, normFactor, radius);
}

void SurfaceGraph::buildGeometry()
{
    for (SurfaceFace& face : m_faces) face.surface->buildVertices();

    for (size_t i = 0; i < m_edges.size(); i++) blendEdgeTangentSpace(i);
    for (size_t i = 0; i < m_vertices.size(); i++) blendVertexTangentSpace(i);
}