#pragma once

#include "Render/StaticObject.h"
#include <vector>

namespace Render
{

struct CompositeNode
{
    ResourcePtr<Model> model;
    mat4 mat;

    CompositeNode(Model* model, const mat4& mat) 
    : model(model) 
    , mat(mat)
    {
    }
};

class CompositeObject : public StaticObject
{
private:
    std::vector<CompositeNode> m_nodes;

protected:
    void updateNodeData();

public:
    CompositeObject();
    CompositeObject(Model* model, bool dynamic = false);
    CompositeObject(const vec3& pos, const vec3& rot, Model* model = nullptr, bool dynamic = false);
    CompositeObject(const vec3& pos, const mat3& mat, Model* model = nullptr, bool dynamic = false);

    ~CompositeObject() {}

    void updateData();

    void addNode(const vec3& pos, const mat3& mat, Model* model);
    void setNodeTransform(size_t node, const mat4& mat);
    void setNodeMat(size_t node, const mat4& mat) { m_nodes[node].mat = mat; }
};

} //namespace Render