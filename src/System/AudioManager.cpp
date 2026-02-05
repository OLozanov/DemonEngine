#include "AudioManager.h"

#include "System/ErrorMsg.h"

#undef min
#undef max

AudioManager AudioManager::Instance;

AudioManager::AudioManager()
: m_ahead(InvalidId)
, m_atail(InvalidId)
, m_alloc_ptr(0)
{
    ThrowIfFailed(CoInitializeEx(nullptr, COINIT_MULTITHREADED));
    ThrowIfFailed(XAudio2Create(&m_xaudio, 0, XAUDIO2_DEFAULT_PROCESSOR));
    ThrowIfFailed(m_xaudio->CreateMasteringVoice(&m_masterVoice));

    for (int i = 0; i < AudioChannels; i++)
    {
        PlayData& playData = m_channels[i];

        WAVEFORMATEX waveformat = {};

        waveformat.wFormatTag = WAVE_FORMAT_PCM;
        waveformat.nChannels = 1;
        waveformat.nSamplesPerSec = SampleRate;
        waveformat.nAvgBytesPerSec = SampleRate * 2;
        waveformat.nBlockAlign = 2;
        waveformat.wBitsPerSample = 16;

        ThrowIfFailed(m_xaudio->CreateSourceVoice(&playData.voice, &waveformat, 0, 2.0f, this));

        m_cpool[i] = i;
    }
}

AudioManager::~AudioManager()
{
    for (int i = 0; i < AudioChannels; i++)
    {
        IXAudio2SourceVoice* voice = m_channels[i].voice;
        if(voice) voice->DestroyVoice();
    }

    m_masterVoice->DestroyVoice();
}

float AudioManager::getVolume()
{
    float volume;
    m_masterVoice->GetVolume(&volume);

    return volume;
}

void AudioManager::setVolume(float volume)
{
    m_masterVoice->SetVolume(volume);
}

void AudioManager::setSoundVolume(PlayId playId, float volume)
{
    PlayData& playData = m_channels[playId];

    if (playData.playing) playData.voice->SetVolume(volume);
}

void AudioManager::setSoundVolume(PlayId playId, const vec3& pos)
{
    PlayData& playData = m_channels[playId];

    float volume = calculateVolume(pos);

    if (playData.playing) playData.voice->SetVolume(volume);
}

int AudioManager::allocateChannel()
{
    if (m_alloc_ptr < AudioChannels)
    {
        int channel = m_cpool[m_alloc_ptr++];

        if (m_ahead == InvalidId)
        {
            m_ahead = channel;
            m_channels[channel].prev = InvalidId;
        }
        else
        {
            m_channels[m_atail].next = channel;
            m_channels[channel].prev = m_atail;
        }

        m_channels[channel].next = InvalidId;
        m_atail = channel;

        return channel;
    }
    
    //replace oldest sound (but not looping sound)
    int channel = m_ahead;

    while (channel != InvalidId)
    {
        PlayData& playData = m_channels[channel];

        if (playData.loop) channel = playData.next;
        else return channel;
    }

    return channel;
}

void AudioManager::freeChannel(int channel)
{
    int onext = m_channels[channel].next;
    int oprev = m_channels[channel].prev;

    if (oprev != InvalidId) m_channels[oprev].next = onext;
    else m_ahead = onext;

    if (onext != InvalidId) m_channels[onext].prev = oprev;
    else m_atail = oprev;

    m_cpool[--m_alloc_ptr] = channel;
}

AudioManager::PlayId AudioManager::play(const Sound* sound, float volume, bool loop)
{
    int channel = allocateChannel();

    PlayData& playData = m_channels[channel];

    if (playData.playing)
    {
        playData.playing = false;
        playData.flush = true;

        playData.voice->Stop(0);
        playData.voice->FlushSourceBuffers();
    }

    XAUDIO2_BUFFER bufferDesc = {};

    bufferDesc.Flags = XAUDIO2_END_OF_STREAM;
    bufferDesc.AudioBytes = sound->bytes();
    bufferDesc.pAudioData = sound->data();
    bufferDesc.PlayBegin = 0;
    bufferDesc.PlayLength = 0;
    bufferDesc.LoopBegin = XAUDIO2_NO_LOOP_REGION;
    bufferDesc.LoopLength = 0;
    bufferDesc.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
    bufferDesc.pContext = &playData;

    if (playData.voice->SubmitSourceBuffer(&bufferDesc) == S_OK)
    {
        playData.playing = true;
        playData.loop = loop;
        playData.voice->SetVolume(volume);
        playData.voice->Start();
    }

    return channel;
}

AudioManager::PlayId AudioManager::play(const Sound* sound, const vec3& pos, bool loop)
{
    float volume = calculateVolume(pos);

    if (volume < VolumeThreshold) return -1;

    return play(sound, volume, loop);
}

void AudioManager::stop(PlayId playId)
{
    if (playId >= AudioChannels) return;
    if (playId < 0) return;

    if (!m_channels[playId].playing) return;

    PlayData& playData = m_channels[playId];

    playData.voice->Stop();
    playData.voice->FlushSourceBuffers();
    playData.playing = false;
    playData.flush = true;

    freeChannel(playId);
}

void AudioManager::setListenerPos(const vec3& pos)
{ 
    m_listenerPos = pos; 
}

float AudioManager::calculateVolume(const vec3& pos)
{
    constexpr float full_volume_dist = 1.0f;
    const float attenuation_factor = log(VolumeThreshold) / AttenuationDist;

    float dist = (pos - m_listenerPos).length();

    if (dist < full_volume_dist) return 1.0;

    float attenuation = (dist - full_volume_dist) * attenuation_factor;

    return exp(attenuation);

    //return std::max(0.0f, (attenuation_dist - (dist - full_volume_dist)) / attenuation_dist);
}

void AudioManager::OnBufferEnd(void* context)
{
    // Dirty hack in fact. But should work.
    size_t channel = (reinterpret_cast<ptrdiff_t>(context) - reinterpret_cast<ptrdiff_t>(&m_channels[0])) / sizeof(PlayData);

    if (m_channels[channel].flush)
    {
        m_channels[channel].flush = false;
        return;
    }

    if (m_channels[channel].playing) freeChannel(channel);
    m_channels[channel].playing = false;
}