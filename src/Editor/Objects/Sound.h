#pragma once

#include "Object.h"

class AmbientSound : public SpriteObject
{
public:
    AmbientSound();
    AmbientSound(float volume, const std::string& source);

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void write(FILE* file) const override;

private:
    float m_volume;
    std::string m_source;

    static const TypeInfo SoundTypeInfo;
};