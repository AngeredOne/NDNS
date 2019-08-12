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

    bool RecordAudio(int16* data);
    void PlayAudio(Sint16 *data);

private:
    SDLAudioManager(SDLAudioManager const &) = delete;
    SDLAudioManager &operator=(SDLAudioManager const &) = delete;
    SDLAudioManager();
    ~SDLAudioManager();

    SDL_AudioSpec AUDIO_SPEC;

    std::list<std::shared_ptr<AbstractSoundProcessor>> inputProcessors;
    SDL_AudioDeviceID input = -1;
    std::list<std::shared_ptr<AbstractSoundProcessor>> outputProcessors;
    SDL_AudioDeviceID output = -1;
};