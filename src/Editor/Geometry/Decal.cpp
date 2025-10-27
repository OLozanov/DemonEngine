#include "Decal.h"

Render::VertexBuffer Decal::m_wireVBuffer;
Render::IndexBuffer Decal::m_wireIBuffer;

Decal::Decal(Material* material)
: m_material(material)
, m_selected(false)
{
    m_size.x = material->img[0]->width * 0.001f;
    m_size.y = material->img[0]->height * 0.001f;
    m_size.z = 0.05f;

    InitBBoxGeometry();
}

void Decal::InitBBoxGeometry()
{
    if (!m_wireVBuffer.empty()) return;

    std::vector<vec3> vertices = { {-1.0f, -1.0f, -1.0f},
                                   {1.0f, -1.0f, -1.0f},
                                   {1.0f, -1.0f, 1.0f},
                                   {-1.0f, -1.0f, 1.0f},
                                   {-1.0f, 1.0f, -1.0f},
                                   {1.0f, 1.0f, -1.0f},
                                   {1.0f, 1.0f, 1.0f},
                                   {-1.0f, 1.0f, 1.0f} };


    std::vector<uint16_t> indices = { { 0, 1, 1, 2, 2, 3, 3, 0,
                                        4, 5, 5, 6, 6, 7, 7, 4,
                                        0, 4, 1, 5, 2, 6, 3, 7 } };

    m_wireVBuffer.setData(vertices.data(), vertices.size());
    m_wireIBuffer.setData(indices.data(), indices.size());
}

void Decal::buildGeometry()
{
    const vec3& xdir = m_orientation[0];
    const vec3& ydir = m_orientation[1];

    m_geometry.push({ { - xdir + ydir }, { 0, 1 }, {}, {}, {} });
    m_geometry.push({ { xdir + ydir }, { 1, 1 }, {}, {}, {} });
    m_geometry.push({ { - xdir - ydir }, { 0, 0 }, {}, {}, {} });
    m_geometry.push({ { xdir - ydir }, { 1, 0 }, {}, {}, {} });
}

void Decal::buildGeometry(const vec4* planes, const EditPolygon* poly, bool negative)
{
    const vec3& xdir = m_orientation[0];
    const vec3& ydir = m_orientation[1];

    std::vector<Vertex> verts[2];

    std::vector<Vertex> right;

    for (int i = 0; i < 4; i++)
    {
        const std::vector<Vertex>& in = i == 0 ? poly->vertices : verts[(i - 1) % 2];
        std::vector<Vertex>& out = verts[i % 2];

        out.clear();

        SplitPoly(planes[i], in, out, right);
    
        right.clear();
    }

    for (int i = 0; i < verts[1].size(); i++)
    {
        //size_t v = ((i % 2 == 0) ? verts[1].size() - 1 - i / 2 : i / 2);

        size_t v = negative ?
                    ((i % 2 == 0) ? i / 2 : verts[1].size() - 1 - i / 2) :
                    ((i % 2 == 0) ? verts[1].size() - 1 - i / 2 : i / 2);

        vec3 pos = verts[1][v].position - m_pos;

        vec2 tcoord = { (pos * xdir) / m_size.x * 0.5f + 0.5f, (pos * ydir) / m_size.y * 0.5f + 0.5f };
        Vertex vert = {verts[1][v].position, tcoord, verts[1][v].normal, verts[1][v].tangent, verts[1][v].binormal };

        m_geometry.push(vert);
    }
}

void Decal::buildGeometry(const std::vector<EditPolygon*>& polys, bool negative)
{
    const vec3& xdir = m_orientation[0];
    const vec3& ydir = m_orientation[1];

    vec4 planes[4] = { vec4(xdir, -(m_pos * xdir + m_size.x)),
                       vec4(-xdir, -(m_pos * -xdir + m_size.x)),
                       vec4(ydir, -(m_pos * ydir + m_size.y)),
                       vec4(-ydir, -(m_pos * -ydir + m_size.y)) };

    for (const EditPolygon* poly : polys) buildGeometry(planes, poly, negative);
}

void Decal::display(Render::CommandList& commandList) const
{
    //commandList.setConstant(1, mat4::Translate(m_pos));
    commandList.setConstant(1, mat4());
    commandList.bind(3, m_material->maps[Material::map_diffuse]);
    commandList.bindVertexBuffer(m_geometry);
    commandList.draw(m_geometry.size());
}

void Decal::displayWire(Render::CommandList& commandList) const
{
    commandList.setConstant(1, mat4::Translate(m_pos) * m_orientation * mat4::Scale(m_size));
    commandList.bindVertexBuffer(m_wireVBuffer);
    commandList.bindIndexBuffer(m_wireIBuffer);

    commandList.setConstant(2, vec4(1.0, 1.0, 1.0, 1.0));

    commandList.drawIndexed(8 * 3);
}