#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <unistd.h>

class SDLAudioManager
{
public:
    static SDLAudioManager &Get();

    bool SetupInput(const char *deviceName, SDL_AudioSpec spec);
    bool SetupOutput(const char *deviceName, SDL_AudioSpec spec);

    Uint8 *RecordAudio(int len);
    void PlayAudio(Uint8 *data, int len);

private:

    SDLAudioManager(SDLAudioManager const &) = delete;
    SDLAudioManager &operator=(SDLAudioManager const &) = delete;
    SDLAudioManager()
    {
        SDL_Init(SDL_INIT_AUDIO);
        SDL_AudioInit(SDL_GetAudioDriver(0));
    }

    ~SDLAudioManager()
    {
        SDL_CloseAudio();
    }

    SDL_AudioDeviceID input;
    SDL_AudioDeviceID output;
};