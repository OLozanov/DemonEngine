#include "Render/StaticObject.h"

namespace Render
{

StaticObject::StaticObject(Model* model, bool dynamic)
: m_model(model)
{
    m_dynamic = dynamic;
    initData();
}

StaticObject::StaticObject(const vec3& pos, const vec3& rot, Model* model, bool dynamic)
: m_model(model)
{
    m_mat = mat4::Translate(pos) * mat4::Rotate(rot.x/180.0*math::pi, rot.y/180.0*math::pi, rot.z/180.0*math::pi);
    m_dynamic = dynamic;
    initData();
}

StaticObject::StaticObject(const vec3& pos, const mat3& mat, Model* model, bool dynamic)
: m_model(model)
{
    m_mat = mat4::Translate(pos) * mat4(mat);
    m_dynamic = dynamic;
    initData();
}

void StaticObject::initData()
{
    m_bbox = m_model->bbox();

    std::vector<const DisplayData*> displayDataRegular;
    std::vector<const DisplayData*> displayDataTransparent;
    std::vector<const DisplayData*> displayDataEmissive;

    for (const auto& mesh : m_model->meshes())
    {
        switch (mesh.material->type)
        {
        case Material::material_regular:
            displayDataRegular.push_back(&mesh);
        break;

        case Material::material_transparent:
            displayDataTransparent.push_back(&mesh);
        break;

        case Material::material_emissive:
            displayDataEmissive.push_back(&mesh);
        break;
        }
    }

    if (!displayDataRegular.empty())
    {
        m_displayData.emplace_back(DisplayBlock::display_regular,
                                   &m_mat,
                                   m_model->vertexBuffer(),
                                   m_model->indexBuffer(),
                                   displayDataRegular);
    }

    if (!displayDataTransparent.empty())
    {
        m_displayData.emplace_back(DisplayBlock::display_transparent,
                                   &m_mat,
                                   m_model->vertexBuffer(),
                                   m_model->indexBuffer(),
                                   displayDataTransparent);
    }

    if (!displayDataEmissive.empty())
    {
        m_displayData.emplace_back(DisplayBlock::display_emissive,
                                   &m_mat,
                                   m_model->vertexBuffer(),
                                   m_model->indexBuffer(),
                                   displayDataEmissive);
    }
}

} //namespace render
