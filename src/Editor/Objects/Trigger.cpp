#include "Trigger.h"

const TypeInfo Trigger::TriggerTypeInfo = { "Trigger",
                                             nullptr,
                                             {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(Trigger, m_pos), 0},
                                              {"size", TypeInfoMember::Type::Vec3, 0, offsetof(Trigger, m_size), 0},
                                              {"object", TypeInfoMember::Type::String, 0, offsetof(Trigger, m_objectId), 0}}
};

static constexpr vec3 VolumeColor = { 1.0f, 0.7f, 0.0f };

Trigger::Trigger()
: BoxObject({ 2.0f, 1.0f, 2.0f }, VolumeColor)
{
}

Trigger::Trigger(const vec3& pos, const vec3& size, const std::string& id)
: BoxObject(size, VolumeColor)
, m_objectId(id)
{
    m_pos = pos;
}

ObjectType Trigger::type() const
{
    return ObjectType::Trigger;
}

const TypeInfo& Trigger::getTypeInfo() const
{
    return TriggerTypeInfo;
}

Object* Trigger::clone() const
{
    return new Trigger(*this);
}

void Trigger::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_size, sizeof(vec3), 1, file);

    uint16_t len = m_objectId.size();

    fwrite(&len, sizeof(uint16_t), 1, file);
    if (len) fwrite(m_objectId.c_str(), sizeof(char), len, file);
}