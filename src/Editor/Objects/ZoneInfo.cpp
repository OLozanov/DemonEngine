#include "ZoneInfo.h"

const EnumInfo ZoneTypeEnumInfo = { {"Regular", 0},
                                    {"Water", 1},
                                    {"Lava", 2},
                                    {"Slime", 2} };

const TypeInfo ZoneInfo::ZoneTypeInfo = { "Zone Info",
                                           nullptr,
                                           {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(ZoneInfo, m_pos), 0},
                                            {"type", TypeInfoMember::Type::Enum, &ZoneTypeEnumInfo, offsetof(ZoneInfo, m_type), 0}}
                                        };

ZoneInfo::ListType ZoneInfo::m_list;

ZoneInfo::ZoneInfo()
: ZoneInfo({}, ZoneType::Regular)
{
}

ZoneInfo::ZoneInfo(const vec3& pos, ZoneType type)
: SpriteObject(ResourceManager::GetImage("Editor\\info_ico.dds")->handle, 0.5)
, m_type(type)
{
    m_pos = pos;

    m_list.emplace_back(this);
    m_listKey = std::prev(m_list.end());
}

ZoneInfo::~ZoneInfo()
{
    m_list.erase(m_listKey);
}

ObjectType ZoneInfo::type() const
{
    return ObjectType::ZoneInfo;
}

const TypeInfo& ZoneInfo::getTypeInfo() const
{
    return ZoneTypeInfo;
}

Object* ZoneInfo::clone() const
{
    return new ZoneInfo(*this);
}

void ZoneInfo::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_type, sizeof(ZoneType), 1, file);
}