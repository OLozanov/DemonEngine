#include "Formats.h"
#include "Resources/Sound.h"
#include "System/AudioManager.h"

#include "math/constants.h"

#undef min
#undef max

struct WaveFormat
{
    uint16_t format;
    uint16_t channels;
    uint32_t samplesPerSec;
    uint32_t avgBytesPerSec;
    uint16_t blockAlign;
    uint16_t bitsPerSample;
};

float sinc(float x)
{   
    if (fabsf(x) < math::eps) return 1.0f;

    return sin(math::pi * x) / math::pi / x;
}

void resample(std::vector<int16_t>& dst, std::vector<int16_t>& src, uint32_t srcRate)
{
    float freqRatio = (float)AudioManager::SampleRate / srcRate;

    dst.resize(ceil(src.size() * freqRatio));

    for (int i = 0; i < dst.size(); i++)
    {
        constexpr int ksize = 4;

        float pos = i / freqRatio;
        int ipos = round(pos);

        int start = std::max(0, ipos - ksize);
        int end = std::min((int)src.size() - 1, ipos + ksize);

        float newval = 0;

        for (int k = start; k <= end; k++)
        {
            float magnitude = src[k];
            float x = pos - (float)k;

            newval += sinc(x) * magnitude;
        }

        dst[i] = newval;
    }
}

void mergestereo(std::vector<int16_t>& src)
{
    size_t size = src.size() / 2;

    std::vector<int16_t> dst(size);

    for (int i = 0; i < size; i++)
    {
        float val1 = src[i * 2];
        float val2 = src[i * 2 + 1];

        dst[i] = (val1 + val2) * 0.5;
    }

    std::swap(dst, src);
}

void readData(std::vector<int16_t>& out, int size, int bps, FILE* file)
{
    for (int i = 0; i < size; i++)
    {
        uint8_t byte;
        uint32_t value = 0;

        for (int b = 0; b < bps; b++)
        {
            fread(&byte, sizeof(uint8_t), 1, file);

            value |= byte;
            value = value << 8;
        }

        out[i] = value;
    }
}

Sound* LoadWav(const std::string& fname)
{
    FILE* file;

    constexpr uint32_t Riff = 'FFIR';
    constexpr uint32_t Data = 'atad';
    constexpr uint32_t Fmt = ' tmf';
    constexpr uint32_t Wave = 'EVAW';

    errno_t error = fopen_s(&file, fname.c_str(), "rb");

    if (error)
        return nullptr;

    Sound* sound = nullptr;

    std::vector<int16_t> data;
    WaveFormat waveformat;

    uint32_t fileType;
    uint32_t chunkId;

    while (fread(&chunkId, sizeof(uint32_t), 1, file))
    {
        uint32_t size;

        fread(&size, sizeof(uint32_t), 1, file);

        switch (chunkId)
        {
        case Riff:
            fread(&fileType, sizeof(uint32_t), 1, file);
        break;
        case Fmt:
            fread(&waveformat, sizeof(WaveFormat), 1, file);
        break;
        case Data:
            if (waveformat.bitsPerSample == 16)
            {
                data.resize(ceil(size / 2));
                fread(data.data(), sizeof(uint8_t), size, file);
            }
            else
            {
                int bps = waveformat.bitsPerSample / 8;

                data.resize(ceil(size / bps));
                readData(data, size / bps, bps, file);
            }
        break;
        default:
            fseek(file, size, SEEK_CUR);
        }
    }

    if (fileType == Wave)
    {
        sound = new Sound;

        if (waveformat.channels == 2)
            mergestereo(data);

        if (waveformat.samplesPerSec == AudioManager::SampleRate)
            sound->samples = std::move(data);
        else
            resample(sound->samples, data, waveformat.samplesPerSec);
    }

    fclose(file);

    return sound;
}