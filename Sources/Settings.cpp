#include "Settings.h"
#include "SDLAudioManager.h"

Loudness::Loudness(float _volume)
{
    SetVolume(_volume);
}

void Loudness::SetVolume(int percentage)
{
    if (percentage <= MIN_VOLUME)
    {
        volume = 0;
        return;
    }
    if (percentage >= MaxVolumeValue())
    {
        volume = MaxVolumeValue();
        return;
    }
    volume = percentage;
}

int Loudness::GetVolume() const
{
    return volume;
}

float Loudness::GetVolumeCoef() const
{
    return volume / 100.f;
}

constexpr int Loudness::MaxVolumeValue()
{
    return MAX_VOLUME * 100;
}

int Loudness::GetThreshold() const {
    return threshold;
}

void Loudness::SetThreshold(int perc) {
    if (perc > MAX_THRESHOLD) {
        threshold = MAX_THRESHOLD;
        return;
    }
    if (perc < MIN_THRESHOLD) {
        threshold = MIN_THRESHOLD;
        return;
    }
    threshold = perc;
}

float Loudness::GetThresholdCoef() const {
    return threshold * 120.f;
}



Settings::Settings()
{
    SDL_zero(config);
    config.freq = 44100;
    config.format = AUDIO_S16;
    config.channels = 1;
    config.samples = 1024;
    config.callback = nullptr;
}

void Settings::SetupFromConsole()
{
    std::cout << "Select devices" << std::endl;

    std::cout << "OUTPUT:" << std::endl;
    for (int i = 0; i < SDL_GetNumAudioDevices(0); ++i)
        std::cout << i << ": " << SDL_GetAudioDeviceName(i, 0) << std::endl;

    std::cout << "option: ";

    int index;
    std::string input;
    getline(std::cin, input);
    index = atoi(input.c_str());
    auto outputDeviceName = SDL_GetAudioDeviceName(index, 0);

    std::cout << std::endl;

    std::cout << "INPUT:" << std::endl;
    for (int i = 0; i < SDL_GetNumAudioDevices(1); ++i)
        std::cout << i << ": " << SDL_GetAudioDeviceName(i, 1) << std::endl;

    std::cout << "option: ";
    getline(std::cin, input);
    index = atoi(input.c_str());
    auto inputDeviceName = SDL_GetAudioDeviceName(index, 1);

    std::cout << std::endl
              << "Result:\n";

    if (SDLAudioManager::Get().SetupInput(inputDeviceName, config))
        std::cout << "INPUT OK " << inputDeviceName << std::endl;
    else
        std::cout << "INPUT ERROR" << std::endl;
    if (SDLAudioManager::Get().SetupOutput(outputDeviceName, config))
        std::cout << "OUTPUT OK " << outputDeviceName << std::endl;
    else
        std::cout << "OUTPUT ERROR" << std::endl;

    std::cout << std::endl;
}