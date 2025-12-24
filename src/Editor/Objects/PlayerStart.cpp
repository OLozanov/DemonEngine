#include "PlayerStart.h"

const TypeInfo PlayerStart::PlayerStartTypeInfo = { "Player Start",
                                                    nullptr,
                                                    {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(PlayerStart, m_pos), 0},
                                                     {"angle", TypeInfoMember::Type::Angle, 0, offsetof(PlayerStart, m_ang), 0}}
};

PlayerStart::PlayerStart()
: OrientedBoxObject({0.25f, 0.5f, 0.25f}, {1.0f, 1.0f, 0.0f}, { 1.0f, 0.7f, 0.0f })
{
}

ObjectType PlayerStart::type() const
{
    return ObjectType::PlayerStart;
}

const TypeInfo& PlayerStart::getTypeInfo() const
{
    return PlayerStartTypeInfo;
}

Object* PlayerStart::clone() const
{
    return new PlayerStart(*this);
}

void PlayerStart::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_ang, sizeof(float), 1, file);
}