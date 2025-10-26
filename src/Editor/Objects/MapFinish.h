#pragma once

#include "Object.h"

class MapFinish : public BoxObject
{
public:

    MapFinish();
    MapFinish(const vec3& size, const std::string& map);

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void write(FILE* file) const override;

private:

    std::string m_nextmap;

    static const TypeInfo MapFinishTypeInfo;
};