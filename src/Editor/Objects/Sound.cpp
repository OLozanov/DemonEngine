#include "Sound.h"

const TypeInfo AmbientSound::SoundTypeInfo = { "Sound",
                                                nullptr,
                                                {{"pos", TypeInfoMember::Type::Vec3, 0, offsetof(AmbientSound, m_pos), 0},
                                                 {"voulme", TypeInfoMember::Type::Float, 0, offsetof(AmbientSound, m_volume), 0},
                                                 {"source", TypeInfoMember::Type::String, 0, offsetof(AmbientSound, m_source), 0}}
};

AmbientSound::AmbientSound()
: AmbientSound(1.0, std::string())
{
}

AmbientSound::AmbientSound(float volume, const std::string& source)
: SpriteObject(ResourceManager::GetImage("Editor\\sound_ico.dds")->handle, 0.5)
, m_volume(volume)
, m_source(source)
{
}

ObjectType AmbientSound::type() const
{
    return ObjectType::AmbientSound;
}

const TypeInfo& AmbientSound::getTypeInfo() const
{
    return SoundTypeInfo;
}

Object* AmbientSound::clone() const
{
    return new AmbientSound(*this);
}

void AmbientSound::write(FILE* file) const
{
    fwrite(&m_pos, sizeof(vec3), 1, file);
    fwrite(&m_volume, sizeof(float), 1, file);
    
    uint32_t len = m_source.size();
    fwrite(&len, sizeof(uint32_t), 1, file);
    fwrite(m_source.c_str(), 1, len, file);
}