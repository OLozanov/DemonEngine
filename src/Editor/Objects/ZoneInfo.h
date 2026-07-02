#pragma once

#include "Object.h"
#include "Geometry/Map.h"

#include <list>

class ZoneInfo : public SpriteObject
{
public:
    using ListType = std::list<ZoneInfo*>;

public:
    ZoneInfo();
    ZoneInfo(const vec3& pos, ZoneType type);

    ~ZoneInfo();

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    ZoneType zoneType() const { return m_type; }

    void write(FILE* file) const override;

    static const ListType& List() { return m_list; }

private:
    ListType::iterator m_listKey;
    ZoneType m_type;

    static const TypeInfo ZoneTypeInfo;

    static ListType m_list;
};