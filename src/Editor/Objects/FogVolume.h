#pragma once

#include "Object.h"

class FogVolume : public BoxObject
{
public:
    enum VolumeType : uint8_t
    {
        rectangle_volume = 0,
        ellipse_volume = 1
    };

public:
    FogVolume();
    FogVolume(const vec3& pos, 
              const vec3& size,
              const vec3& color,
              float density,
              VolumeType type,
              bool lighting);

    ObjectType type() const override;
    const TypeInfo& getTypeInfo() const override;

    Object* clone() const override;

    void write(FILE* file) const override;

private:

    VolumeType m_volumeType;
    float m_density;
    bool m_lighting;

    static const TypeInfo FogTypeInfo;
    static const EnumInfo ShapeEnumInfo;
};
