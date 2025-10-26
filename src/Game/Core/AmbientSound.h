#pragma once

#include "Game/GameObject.h"

#include "System/AudioManager.h"
#include "Resources/Resources.h"

namespace GameLogic
{

class AmbientSound : public GameObject
{
public:
    AmbientSound(const vec3& pos, float volume, Sound* sound);

    void update(float dt) override;

private:
    vec3 m_pos;
    float m_volume;
    SoundPtr m_sound;

    AudioManager::PlayId m_playId;
    bool m_playing;
};

} // namespace gamelogic