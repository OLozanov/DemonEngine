#pragma once

#include "Object.h"

class PlayerStart : public OrientedBoxObject
{
public:

    PlayerStart();

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void write(FILE* file) const override;

private:

    static const TypeInfo PlayerStartTypeInfo;
};