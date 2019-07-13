#pragma once
#include <iostream>
#include <SDL2/SDL.h>
#include <unistd.h>
#include <list>
#include <functional>

#include "SoundProcessing.h"
#include "Settings.h"

class AbstractSoundProcessor;

class SDLAudioManager
{
public:
    static SDLAudioManager &Get()
    {
        static SDLAudioManager instance;
        return instance;
    }
    void InitProcessors();

    bool SetupInput(const char *deviceName, SDL_AudioSpec spec);
    bool SetupOutput(const char *deviceName, SDL_AudioSpec spec);

    Sint16 *RecordAudio(int len);
    void PlayAudio(Sint16 *data, int len);
    void StartRecord()
    {
        SDL_PauseAudioDevice(input, 0);
    }

private:
    SDLAudioManager(SDLAudioManager const &) = delete;
    SDLAudioManager &operator=(SDLAudioManager const &) = delete;
    SDLAudioManager() {}

    ~SDLAudioManager()
    {
        SDL_CloseAudio();
    }
    std::list<std::shared_ptr<AbstractSoundProcessor>> inputProcessors;
    SDL_AudioDeviceID input;
    std::list<std::shared_ptr<AbstractSoundProcessor>> outputProcessors;
    SDL_AudioDeviceID output;
};