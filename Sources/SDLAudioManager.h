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
    void StartRecord() {
        SDL_PauseAudioDevice(input, 0);
    }

private:

    SDLAudioManager(SDLAudioManager const &) = delete;
    SDLAudioManager &operator=(SDLAudioManager const &) = delete;
    SDLAudioManager()
    {
    }

    ~SDLAudioManager()
    {
        SDL_CloseAudio();
    }

    SDL_AudioDeviceID input;
    SDL_AudioDeviceID output;
};