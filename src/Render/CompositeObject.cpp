#include "CompositeObject.h"

namespace Render
{

CompositeObject::CompositeObject()
: StaticObject()
{
}

CompositeObject::CompositeObject(Model* model, bool dynamic)
: StaticObject(model, dynamic)
{
}

CompositeObject::CompositeObject(const vec3& pos, const vec3& rot, Model* model, bool dynamic)
: StaticObject(pos, rot, model, dynamic)
{
}

CompositeObject::CompositeObject(const vec3& pos, const mat3& mat, Model* model, bool dynamic)
: StaticObject(pos, mat, model, dynamic)
{
}

void CompositeObject::updateData()
{
    m_displayData.clear();

    initData();
    updateNodeData();
}

void CompositeObject::addNode(const vec3& pos, const mat3& mat, Model* model)
{
    mat4 transform = mat4(mat, pos);

    const CompositeNode& node = m_nodes.emplace_back(model, m_mat * transform);

    const BBox& bbox = m_model->bbox();

    vec3 boxdim = (bbox.max - bbox.min) * 0.5f;
    vec3 boxcenter = (bbox.max + bbox.min) * 0.5f;

    for (size_t i = 0; i < 3; i++)
    {
        float r = boxdim[0] * fabs(transform[0][i]) +
                  boxdim[1] * fabs(transform[1][i]) +
                  boxdim[2] * fabs(transform[2][i]);

        m_bbox.min[i] = std::min(m_bbox.min[i], pos[i] - r);
        m_bbox.max[i] = std::max(m_bbox.max[i], pos[i] + r);
    }
}

void CompositeObject::updateNodeData()
{
    for (const CompositeNode& node : m_nodes)
    {
        std::vector<const DisplayData*> displayDataRegular;
        std::vector<const DisplayData*> displayDataTransparent;
        std::vector<const DisplayData*> displayDataEmissive;

        for (const auto& mesh : node.model->meshes())
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
                &node.mat,
                node.model->vertexBuffer(),
                node.model->indexBuffer(),
                displayDataRegular);
        }

        if (!displayDataTransparent.empty())
        {
            m_displayData.emplace_back(DisplayBlock::display_transparent,
                &node.mat,
                node.model->vertexBuffer(),
                node.model->indexBuffer(),
                displayDataTransparent);
        }

        if (!displayDataEmissive.empty())
        {
            m_displayData.emplace_back(DisplayBlock::display_emissive,
                &node.mat,
                node.model->vertexBuffer(),
                node.model->indexBuffer(),
                displayDataEmissive);
        }
    }
}

void CompositeObject::setNodeTransform(size_t node, const mat4& mat)
{
    m_nodes[node].mat = m_mat * mat;
}

} //namespace Render