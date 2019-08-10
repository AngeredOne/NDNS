#pragma once
#include "NDNS.h"

class AbstractSoundProcessor;


const int AUDIO_BUF = 2048;

class SDLAudioManager
{
public:
    static SDLAudioManager &Get()
    {
        static SDLAudioManager instance;
        return instance;
    }
    void InitProcessors();

    void Start();
    void Stop();

    bool SetupInput(const char *deviceName);
    bool SetupOutput(const char *deviceName);

    Sint16 *RecordAudio();
    void PlayAudio(Sint16 *data);

private:
    SDLAudioManager(SDLAudioManager const &) = delete;
    SDLAudioManager &operator=(SDLAudioManager const &) = delete;
    SDLAudioManager();

    ~SDLAudioManager()
    {
        SDL_CloseAudio();
    }

    SDL_AudioSpec AUDIO_SPEC;

    std::list<std::shared_ptr<AbstractSoundProcessor>> inputProcessors;
    SDL_AudioDeviceID input;
    std::list<std::shared_ptr<AbstractSoundProcessor>> outputProcessors;
    SDL_AudioDeviceID output;
};