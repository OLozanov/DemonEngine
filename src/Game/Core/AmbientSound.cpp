#include "AmbientSound.h"

namespace GameLogic
{

AmbientSound::AmbientSound(const vec3& pos, float volume, Sound* sound)
: m_pos(pos)
, m_volume(volume)
, m_sound(sound)
, m_playing(false)
{
}

AmbientSound::~AmbientSound()
{
    if (m_playing) AudioManager::Stop(m_playId);
}

void AmbientSound::update(float dt)
{
    float volume = AudioManager::GetVolumeLevel(m_pos) * m_volume;
    
    bool play = volume > AudioManager::VolumeThreshold ? true : false;

    if (m_playing != play)
    {
        if (play)
            m_playId = AudioManager::Play(m_sound, volume, true);
        else
            AudioManager::Stop(m_playId);
    }

    if (m_playing && play)
        AudioManager::SetSoundVolume(m_playId, volume);

    m_playing = play;
}

} // namespace gamelogic