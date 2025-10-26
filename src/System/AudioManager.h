#pragma once

#include "Resources/Sound.h"

#include "xaudio2.h"
#include <wrl.h>

#include "math/math3d.h"

using Microsoft::WRL::ComPtr;

class AudioManager : public IXAudio2VoiceCallback
{
public:

    using PlayId = int;
    static constexpr int InvalidId = -1;

    static constexpr uint32_t SampleRate = 44100;

    static AudioManager& GetInstance()
    {
        return Instance;
    }

    static PlayId Play(const Sound* sound, float volume = 1.0, bool loop = false)
    {
        return Instance.play(sound, volume, loop);
    }

    static PlayId Play(const Sound* sound, const vec3& pos, bool loop = false)
    {
        return Instance.play(sound, pos, loop);
    }

    static void Stop(PlayId playId)
    {
        return Instance.stop(playId);
    }

    static void SetListenerPos(const vec3& pos)
    {
        Instance.setListenerPos(pos);
    }

    static float GetVolumeLevel(const vec3& pos)
    {
        return Instance.calculateVolume(pos);
    }

    static void SetSoundVolume(PlayId playId, float volume)
    {
        Instance.setSoundVolume(playId, volume);
    }

    float getVolume();
    void setVolume(float volume);
    void setSoundVolume(PlayId playId, float volume);
    void setSoundVolume(PlayId playId, const vec3& pos);
    PlayId play(const Sound* sound, float volume = 1.0, bool loop = false);
    PlayId play(const Sound* sound, const vec3& pos, bool loop = false);
    void stop(PlayId playId);

    void setListenerPos(const vec3& pos);

    static constexpr float VolumeThreshold = 0.01f;
    static constexpr float AttenuationDist = 12.0f;   // attenuation to the level of VolumeThreshold

private:

    struct PlayData
    {
        IXAudio2SourceVoice* voice = nullptr;

        bool playing = false;
        bool loop = false;

        int prev;
        int next;
    };

    static AudioManager Instance;

    static constexpr int AudioChannels = 8;
    static constexpr uint32_t BufferSize = 1024;

    ComPtr<IXAudio2> m_xaudio;
    IXAudio2MasteringVoice* m_masterVoice;

    std::vector<uint8_t> m_buffer;

    PlayData m_channels[AudioChannels];
    int m_cpool[AudioChannels];

    int m_alloc_ptr;

    int m_ahead;
    int m_atail;

    vec3 m_listenerPos;

    AudioManager();
    ~AudioManager();

    int allocateChannel();
    void freeChannel(int channel);
    float calculateVolume(const vec3& pos);

    void OnVoiceProcessingPassStart(UINT32 BytesRequired) override {}
    void OnVoiceProcessingPassEnd() override {}
    void OnStreamEnd() override {}
    void OnBufferStart(void* context) override {}
    void OnBufferEnd(void* context) override;
    void OnLoopEnd(void* context) override {}
    void OnVoiceError(void* context, HRESULT Error) override {}
};
