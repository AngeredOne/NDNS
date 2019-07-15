#include "SettingsFields.h"
#include "SDLAudioManager.h"

float VolumeControl::GetVolume() const
{
    return volume;
}

void VolumeControl::ApplyValue(std::string value)
{
    float _value = (atoi(value.c_str()) / 100.f);
    if (_value <= MIN_VOLUME)
    {
        volume = 0;
        return;
    }
    if (_value >= MAX_VOLUME)
    {
        volume = MAX_VOLUME;
        return;
    }
    volume = _value;
}

std::string VolumeControl::GetValue() const
{
    return std::to_string((int)(volume * 100.f));
}

void ThresholdControl::ApplyValue(std::string value)
{
    int _value = atoi(value.c_str());
    if (_value > MAX_THRESHOLD)
    {
        threshold = MAX_THRESHOLD;
        return;
    }
    if (_value < MIN_THRESHOLD)
    {
        threshold = MIN_THRESHOLD;
        return;
    }
    threshold = _value;
}
std::string ThresholdControl::GetValue() const
{
    return std::to_string(threshold);
}

float ThresholdControl::GetThreshold() const
{
    return threshold * 120.f;
}

void Device::ApplyValue(std::string value)
{
    for (int i = 0; i < SDL_GetNumAudioDevices(isInput); ++i)
    {
        if (value == SDL_GetAudioDeviceName(i, isInput))
        {
            id = i;
            break;
        }
    }
    if (isInput)
    {
        SDLAudioManager::Get().SetupInput(value.c_str());
    }
    else
    {
        SDLAudioManager::Get().SetupOutput(value.c_str());
    }
}
std::string Device::GetValue() const
{
    if (id == -1)
    {
        return "";
    }
    return SDL_GetAudioDeviceName(id, isInput);
}
