#include "Ladder.h"

const TypeInfo Ladder::LadderTypeInfo = { "Ladder",
                                           nullptr,
                                           {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(Ladder, m_pos), 0},
                                            {"height", TypeInfoMember::Type::Float, 0, offsetof(Ladder, m_size.y), 0},
                                            {"angle", TypeInfoMember::Type::Angle, 0, offsetof(Ladder, m_ang), 0}}
};

Ladder::Ladder()
: OrientedBoxObject({ 0.25f, 1.0f, 0.1f }, { 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f, 0.0f })
{
}

ObjectType Ladder::type() const
{
    return ObjectType::Ladder;
}

const TypeInfo& Ladder::getTypeInfo() const
{
    return LadderTypeInfo;
}

Object* Ladder::clone() const
{
    return new Ladder(*this);
}

void Ladder::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_size.y, sizeof(float), 1, file);
    fwrite(&m_ang, sizeof(float), 1, file);
}