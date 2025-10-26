#include "FogVolume.h"

const EnumInfo FogVolume::ShapeEnumInfo = { {"rectangle", 0},
                                            {"ellipse", 1} };

const TypeInfo FogVolume::FogTypeInfo = { "Fog",
                                             nullptr,
                                             {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(FogVolume, m_pos), 0},
                                              {"size", TypeInfoMember::Type::Vec3, 0, offsetof(FogVolume, m_size), 0},
                                              {"color", TypeInfoMember::Type::Color, 0, offsetof(FogVolume, m_color), 0},
                                              {"density", TypeInfoMember::Type::Float, 0, offsetof(FogVolume, m_density), 0},
                                              {"shape", TypeInfoMember::Type::Enum, &FogVolume::ShapeEnumInfo, offsetof(FogVolume, m_volumeType), 0},
                                              {"lighting", TypeInfoMember::Type::Bool, 0, offsetof(FogVolume, m_lighting), 0}}
                                        };

static constexpr vec3 DefaultColor = { 0.6f, 0.6f, 0.6f };

FogVolume::FogVolume()
: BoxObject({ 2.0f, 1.0f, 2.0f }, DefaultColor)
, m_density(10.0f)
, m_volumeType(rectangle_volume)
, m_lighting(false)
{
}

FogVolume::FogVolume(const vec3& pos,
                     const vec3& size,
                     const vec3& color,
                     float density,
                     VolumeType type,
                     bool lighting)
: BoxObject(size, color)
, m_density(density)
, m_volumeType(type)
, m_lighting(lighting)
{
    m_pos = pos;
}

ObjectType FogVolume::type() const
{
    return ObjectType::FogVolume;
}

const TypeInfo& FogVolume::getTypeInfo() const
{
    return FogTypeInfo;
}

Object* FogVolume::clone() const
{
    return new FogVolume(*this);
}

void FogVolume::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_size, sizeof(vec3), 1, file);
    fwrite(&m_color, sizeof(vec3), 1, file);
    fwrite(&m_density, sizeof(float), 1, file);
    fwrite(&m_volumeType, sizeof(uint8_t), 1, file);
    fwrite(&m_lighting, sizeof(bool), 1, file);
}