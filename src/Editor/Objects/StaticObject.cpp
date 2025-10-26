#include "StaticObject.h"
#include "ObjectFactory.h"
#include "Resources/Resources.h"

const TypeInfo StaticObject::StaticObjectTypeInfo = { "Static Object",
                                                     &Object::ObjectTypeInfo,
                                                    {{"model", TypeInfoMember::Type::Model, 0, offsetof(StaticObject, m_modelName), offsetof(StaticObject, m_model)},
                                                     {"collision", TypeInfoMember::Type::Bool, 0, offsetof(StaticObject, m_collision), 0},
                                                     {"global illumination", TypeInfoMember::Type::Bool, 0, offsetof(StaticObject, m_gi), 0}}
};


StaticObject::StaticObject(const std::string& model, bool collision, bool gi)
: MeshObject(ResourceManager::GetModel(model))
, m_modelName(model)
, m_collision(collision)
, m_gi(gi)
{
}

ObjectType StaticObject::type() const
{ 
    return ObjectType::StaticObject; 
}

const TypeInfo& StaticObject::getTypeInfo() const
{
    return StaticObjectTypeInfo;
}

Object* StaticObject::clone() const
{
    return new StaticObject(*this);
}

void StaticObject::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_mat[0], sizeof(vec3), 1, file);
    fwrite(&m_mat[1], sizeof(vec3), 1, file);

    uint32_t len = m_modelName.size();
    fwrite(&len, sizeof(uint32_t), 1, file);
    fwrite(m_modelName.c_str(), 1, len, file);

    uint8_t flags = m_collision | m_gi << 1;
    fwrite(&flags, sizeof(uint8_t), 1, file);
}