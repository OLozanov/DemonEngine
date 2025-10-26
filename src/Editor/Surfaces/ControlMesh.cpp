#include "ControlMesh.h"

ControlMesh::ControlMesh(size_t xsize, size_t ysize)
: m_xsize(xsize)
, m_ysize(ysize)
{
    size_t size = m_xsize * m_ysize;

    m_vertexBuffer.resize(size);

    initIndices();
}

ControlMesh::ControlMesh(size_t size)
: m_xsize(size)
, m_ysize(0)
{
	size_t verts = size * (size + 1) / 2;

    m_vertexBuffer.resize(verts);

    initTriangularIndices();
}

void ControlMesh::initIndices()
{
    std::vector<uint16_t> indices;

    for (int k = 0; k < m_ysize; k++)
    {
        for (int i = 0; i < m_xsize - 1; i++)
        {
            int hind = k * m_xsize + i;

            indices.push_back(hind);
            indices.push_back(hind + 1);
        }
    }

    for (int k = 0; k < m_ysize - 1; k++)
    {
        for (int i = 0; i < m_xsize; i++)
        {
            int vind = k * m_xsize + i;

            indices.push_back(vind);
            indices.push_back(vind + m_xsize);
        }
    }

    m_indexBuffer.setData(indices.data(), indices.size());
    m_indexNum = indices.size();
}

void ControlMesh::initTriangularIndices()
{
    std::vector<uint16_t> indices;

    int p = 0;
    int v = 0;

    int num = m_xsize - 1;

    for (int k = 0; k < m_xsize; k++, num--)
    {
        int vnext = v + num + 1;

        for (int i = 0; i < num; i++)
        {
            //horizontal lines
            indices.push_back(v + i);
            indices.push_back(v + i + 1);

            //left diagonal lines
            indices.push_back(v + i);
            indices.push_back(vnext + i);

            //right diagonal lines
            indices.push_back(v + i + 1);
            indices.push_back(vnext + i);
        }

        v = vnext;
    }

    m_indexBuffer.setData(indices.data(), indices.size());
    m_indexNum = indices.size();
}

void ControlMesh::display(Render::CommandList& commandList) const
{
    commandList.bindVertexBuffer(m_vertexBuffer);
    commandList.bindIndexBuffer(m_indexBuffer);

    commandList.drawIndexed(m_indexNum);
}

void ControlMesh::displayVertices(Render::CommandList& commandList) const
{
    commandList.bindVertexBuffer(m_vertexBuffer);
    commandList.draw(m_vertexBuffer.size());
}