#include "SoundProcessing.h"

void Volume::ProcessSound(Sint16 *samples, int len)
{
    float volume_setting = GetVolume();
    Sint16 *sample = &samples[0];
    for (int i = 0; i < len; ++i)
    {
        *sample *= volume_setting;
        sample++;
    }
}

void Threshold::ProcessSound(Sint16 *samples, int len)
{
    Sint64 loudness = 0;
    Sint16 *sample = &samples[0];
    for (int i = 0; i < len; ++i)
    {
        loudness += *sample * *sample;
        (*sample)++;
    }
    loudness = pow(loudness / len, 1.f / 2.f);

    sample = &samples[0];
    if (loudness < GetThreshold())
    {
        for (int i = 0; i < len; ++i)
        {
            *sample = 0;
            (*sample)++;
        }
    }
}
