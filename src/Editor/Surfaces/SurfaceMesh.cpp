#include "SurfaceMesh.h"
#include "Editor/Geometry/Geometry.h"

#include <algorithm>

#undef min
#undef max

SurfaceMesh::SurfaceMesh(size_t xsize, size_t ysize)
: m_xsize(xsize)
, m_ysize(ysize)
{
    size_t size = m_xsize * m_ysize;

    m_vertexBuffer.resize(size);

    initIndices();
}

void SurfaceMesh::resize(size_t xsize, size_t ysize)
{
    m_xsize = xsize;
    m_ysize = ysize;

    m_vertexBuffer.resize(xsize * ysize);

    initIndices();
}

void SurfaceMesh::initIndices()
{
    std::vector<uint16_t> indices;
    indices.reserve((m_xsize - 1) * (m_ysize - 1) * 6);

    for (int k = 0; k < m_ysize - 1; k++)
    {
        for (int i = 0; i < m_xsize - 1; i++)
        {
            int ind = k * m_xsize + i;

            indices.push_back(ind);
            indices.push_back(ind + 1);
            indices.push_back(ind + m_xsize);

            indices.push_back(ind + 1);
            indices.push_back(ind + m_xsize + 1);
            indices.push_back(ind + m_xsize);
        }
    }

    m_indexBuffer.setData(indices.data(), indices.size());
    m_indexNum = indices.size();
}

void SurfaceMesh::updateBBox()
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

void SurfaceMesh::moveTexCoordS(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.x += val;
}

void SurfaceMesh::moveTexCoordT(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.y += val;
}

void SurfaceMesh::scaleTexCoordS(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.x *= val;
}

void SurfaceMesh::scaleTexCoordT(float val)
{
    for (int i = 0; i < m_vertexBuffer.size(); i++)	m_vertexBuffer[i].tcoord.y *= val;
}

void SurfaceMesh::rotateTexCoord(float ang)
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

bool SurfaceMesh::pick(const vec3& origin, const vec3& ray, float& dist) const
{
    bool result = false;

    for (int k = 0; k < m_ysize - 1; k++)
    {
        for (int i = 0; i < m_xsize - 1; i++)
        {
            int ind = k * m_xsize + i;

            {
                const vec3& v0 = m_vertexBuffer[ind].position;
                const vec3& v1 = m_vertexBuffer[ind + 1].position;
                const vec3& v2 = m_vertexBuffer[ind + m_xsize].position;

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

            {
                const vec3& v0 = m_vertexBuffer[ind + 1].position;
                const vec3& v1 = m_vertexBuffer[ind + m_xsize + 1].position;
                const vec3& v2 = m_vertexBuffer[ind + m_xsize].position;

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
        }
    }

    return result;
}

void SurfaceMesh::display(Render::CommandList& commandList) const
{
    commandList.bindVertexBuffer(m_vertexBuffer);
    commandList.bindIndexBuffer(m_indexBuffer);

    commandList.drawIndexed(m_indexNum);
}