#pragma once

#include "Render/DisplayObject.h"
#include "Resources/Model.h"

namespace Render
{

class StaticObject : public DisplayObject
{
protected:
    ResourcePtr<Model> m_model;

    void initData();

public:

    StaticObject() {}
    StaticObject(Model* model, bool dynamic = false);
    StaticObject(const vec3& pos, const vec3& rot, Model* model = nullptr, bool dynamic = false);
    StaticObject(const vec3& pos, const mat3& mat, Model* model = nullptr, bool dynamic = false);

    ~StaticObject() {}
};

} //namespace Render