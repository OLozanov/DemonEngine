#include "MapFinish.h"

const TypeInfo MapFinish::MapFinishTypeInfo = { "Player Start",
                                                    nullptr,
                                                    {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(MapFinish, m_pos), 0},
                                                     {"size", TypeInfoMember::Type::Vec3, 0, offsetof(MapFinish, m_size), 0},
                                                     {"next map", TypeInfoMember::Type::String, 0, offsetof(MapFinish, m_nextmap), 0} }
};

MapFinish::MapFinish()
: BoxObject({ 1.0f, 0.5f, 1.0f }, { 0.0f, 0.7f, 1.0f })
{
}

MapFinish::MapFinish(const vec3& size, const std::string& map)
: BoxObject(size, { 0.0f, 0.7f, 1.0f })
, m_nextmap(map)
{
}

ObjectType MapFinish::type() const
{
    return ObjectType::MapFinish;
}

const TypeInfo& MapFinish::getTypeInfo() const
{
    return MapFinishTypeInfo;
}

Object* MapFinish::clone() const
{
    return new MapFinish(*this);
}

void MapFinish::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_size, sizeof(vec3), 1, file);

    uint16_t len = m_nextmap.size();
    fwrite(&len, sizeof(uint16_t), 1, file);
    fwrite(m_nextmap.c_str(), 1, len, file);
}