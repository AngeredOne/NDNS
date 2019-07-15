#pragma once
#include "NDNS.h"

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

    void Start();
    void Stop();

    bool SetupInput(const char *deviceName);
    bool SetupOutput(const char *deviceName);

    Sint16 *RecordAudio(int len);
    void PlayAudio(Sint16 *data, int len);
    void StartRecord()
    {
        SDL_PauseAudioDevice(input, 0);
    }

private:
    SDLAudioManager(SDLAudioManager const &) = delete;
    SDLAudioManager &operator=(SDLAudioManager const &) = delete;
    SDLAudioManager()
    {
        SDL_zero(AUDIO_SPEC);
        AUDIO_SPEC.freq = 44100;
        AUDIO_SPEC.format = AUDIO_S16;
        AUDIO_SPEC.channels = 1;
        AUDIO_SPEC.samples = 1024;
        AUDIO_SPEC.callback = nullptr;
    }

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