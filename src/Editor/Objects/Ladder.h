#pragma once

#include "Objects/Object.h"

class Ladder : public OrientedBoxObject
{
public:
    Ladder();

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void write(FILE* file) const override;

    void setHeight(float height) { m_size.y = height; }

private:

    static const TypeInfo LadderTypeInfo;
};