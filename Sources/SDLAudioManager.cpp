#include "SDLAudioManager.h"
#include <iostream>

SDLAudioManager &SDLAudioManager::Get()
{
    static SDLAudioManager instance;
    return instance;
}

bool SDLAudioManager::SetupInput(const char *deviceName, SDL_AudioSpec spec)
{
    SDL_AudioSpec have;
    input = SDL_OpenAudioDevice(deviceName, 1, &spec, &have, 0);
    if (spec.format != have.format)
    {
        std::cout << "Error when opening input audio" << std::endl;
        return false;
    }
    SDL_PauseAudioDevice(input, 0);
    return true;
}

bool SDLAudioManager::SetupOutput(const char *deviceName, SDL_AudioSpec spec)
{
    SDL_AudioSpec have;
    output = SDL_OpenAudioDevice(deviceName, 0, &spec, &have, 0);
    if (spec.format != have.format)
    {
        std::cout << "Error when opening input audio" << std::endl;
        return false;
    }

    SDL_PauseAudioDevice(output, 0);
    return true;
}

Uint8 *SDLAudioManager::RecordAudio(int len)
{

    Uint8 *data = new Uint8[len];
    auto size = SDL_DequeueAudio(input, data, len);
    SDL_Delay(10);
    if (size == 0)
        return nullptr;
    return data;
}

void SDLAudioManager::PlayAudio(Uint8 *data, int len)
{

    if (data != NULL)
    {

        
        auto count = SDL_QueueAudio(output, data, len);
    }
}