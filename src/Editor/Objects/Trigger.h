#pragma once

#include "Object.h"

class Trigger : public BoxObject
{
public:

    Trigger();
    Trigger(const vec3& pos, const vec3& size, const std::string& id);

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void write(FILE* file) const override;

private:

    std::string m_objectId;

    static const TypeInfo TriggerTypeInfo;
};