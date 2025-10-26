#pragma once

#include "Object.h"

class StaticObject : public MeshObject
{
public:

    enum Flags
    {
        flag_collision = 1,
        flag_gi = 2
    };

public:
    StaticObject(const std::string& model, bool collision = true, bool gi = false);

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void write(FILE* file) const override;

private:
    std::string m_modelName;
    bool m_collision;
    bool m_gi;

    static const TypeInfo StaticObjectTypeInfo;
};