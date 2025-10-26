#include "TriangularMesh.h"
#include "Editor/Geometry/Geometry.h"

#include <algorithm>

#undef min
#undef max

TriangularMesh::TriangularMesh(size_t res)
: m_res(res)
{
    size_t size = res * (res + 1) / 2;

    m_vertexBuffer.resize(size);

    initIndices();
}

void TriangularMesh::resize(size_t res)
{
    m_res = res;

    size_t size = res * (res + 1) / 2;

    m_vertexBuffer.resize(size);

    initIndices();
}

void TriangularMesh::initIndices()
{
    size_t tri1 = (m_res) * (m_res + 1) / 2;
    size_t tri2 = (m_res - 1) * (m_res) / 2;

    size_t trinum = tri1 + tri2;

    std::vector<uint16_t> indices(trinum * 3);

    size_t p = 0;
    size_t v = 0;

    size_t num = m_res - 1;

    for (size_t k = 0; k < m_res; k++, num--)
    {
        size_t vnext = v + num + 1;

        for (size_t i = 0; i < num; i++)
        {
            size_t v1 = v + i;
            size_t v2 = vnext + i;

            indices[p] = v1;
            p++;

            indices[p] = v2;
            p++;

            indices[p] = v1 + 1;
            p++;
        }

        v = vnext;
    }

    v = 0;
    num = m_res - 1;

    for (size_t k = 0; k < m_res - 1; k++, num--)
    {
        size_t vnext = v + num + 1;

        for (size_t i = 0; i < num - 1; i++)
        {
            size_t v1 = v + i;
            size_t v2 = vnext + i;

            indices[p] = v2;
            p++;

            indices[p] = v2 + 1;
            p++;

            indices[p] = v1 + 1;
            p++;
        }

        v = vnext;
    }

    m_indexBuffer.setData(indices.data(), indices.size());
    m_indexNum = indices.size();
}

void TriangularMesh::updateBBox()
{
    m_bbox = {};

    m_bbox.min = m_vertexBuffer[0].position;
    m_bbox.max = m_vertexBuffer[0].position;

    for (int i = 1; i < m_vertexBuffer.size(); i++)
    {
        const vec3& vert = m_vertexBuffer[i].position;

        m_bbox.min.x = std::min(m_bbox.min.x, vert.x);
        m_bbox.min.y = std::min(m_bbox.min.y, vert.y);
        m_bbox.min.z = std::min(m_bbox.min.z, vert.z);

        m_bbox.max.x = std::max(m_bbox.max.x, vert.x);
        m_bbox.max.y = std::max(m_bbox.max.y, vert.y);
        m_bbox.max.z = std::max(m_bbox.max.z, vert.z);
    }
}

void TriangularMesh::moveTexCoordS(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.x += val;
}

void TriangularMesh::moveTexCoordT(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.y += val;
}

void TriangularMesh::scaleTexCoordS(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.x *= val;
}

void TriangularMesh::scaleTexCoordT(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.y *= val;
}

void TriangularMesh::rotateTexCoord(float ang)
{
    float Cos = cos(ang / 180.0f * math::pi);
    float Sin = sin(ang / 180.0f * math::pi);

    for (int i = 0; i < m_vertexBuffer.size(); i++)
    {
        vec2 tc = m_vertexBuffer[i].tcoord;

        m_vertexBuffer[i].tcoord.x = tc.x * Cos - tc.y * Sin;
        m_vertexBuffer[i].tcoord.y = tc.x * Sin + tc.y * Cos;
    }
}

bool TriangularMesh::pick(const vec3& origin, const vec3& ray, float& dist) const
{
    bool result = false;

    size_t num = m_res;
    uint16_t v = 0;

    for (size_t k = 0; k < m_res; k++, num--)
    {
        uint16_t vnext = v + num + 1;

        for (size_t i = 0; i < num; i++)
        {
            const vec3& v0 = m_vertexBuffer[v + i].position;
            const vec3& v1 = m_vertexBuffer[vnext + i].position;
            const vec3& v2 = m_vertexBuffer[v + i + 1].position;

            float d;

            if (TriangleIntersect(v0, v1, v2, origin, ray, d))
            {
                if (!result || d < dist)
                {
                    dist = d;
                    result = true;
                }
            }
        }

        v = vnext;
    }

    num = m_res;
    v = 0;

    for (int k = 0; k < m_res - 1; k++, num--)
    {
        uint16_t vnext = v + num + 1;

        for (int i = 0; i < num - 1; i++)
        {
            const vec3& v0 = m_vertexBuffer[vnext + i].position;
            const vec3& v1 = m_vertexBuffer[vnext + i + 1].position;
            const vec3& v2 = m_vertexBuffer[v + i + 1].position;

            float d;

            if (TriangleIntersect(v0, v1, v2, origin, ray, d))
            {
                if (!result || d < dist)
                {
                    dist = d;
                    result = true;
                }
            }
        }

        v = vnext;
    }


    return result;
}

void TriangularMesh::display(Render::CommandList& commandList) const
{
    commandList.bindVertexBuffer(m_vertexBuffer);
    commandList.bindIndexBuffer(m_indexBuffer);

    commandList.drawIndexed(m_indexNum);
}