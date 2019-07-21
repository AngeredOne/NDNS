#include "SDLAudioManager.h"
#include "SoundProcessing.h"
#include "Settings.h"
#include "SettingsFields.h"

SDLAudioManager::SDLAudioManager() {
    SDL_zero(AUDIO_SPEC);
    AUDIO_SPEC.freq = 48000;
    AUDIO_SPEC.format = AUDIO_S16;
    AUDIO_SPEC.channels = 1;
    AUDIO_SPEC.samples = AUDIO_BUF;
    AUDIO_SPEC.callback = nullptr;
}

void SDLAudioManager::InitProcessors()
{
    auto controller = Settings::Get().GetField(S_VOLUME_IN);
    inputProcessors.push_back(std::make_shared<Volume>(std::bind(&VolumeControl::GetVolume, static_cast<VolumeControl *>(controller.get()))));

    controller = Settings::Get().GetField(S_THRESHOLD_IN);
    inputProcessors.push_back(std::make_shared<Threshold>(std::bind(&ThresholdControl::GetThreshold, static_cast<ThresholdControl *>(controller.get()))));

    controller = Settings::Get().GetField(S_VOLUME_OUT);
    outputProcessors.push_back(std::make_shared<Volume>(std::bind(&VolumeControl::GetVolume, static_cast<VolumeControl *>(controller.get()))));
}

bool SDLAudioManager::SetupInput(const char *deviceName)
{
    SDL_AudioSpec have;
    input = SDL_OpenAudioDevice(deviceName, 1, &AUDIO_SPEC, &have, 0);
    if (AUDIO_SPEC.format != have.format)
    {
        std::cout << "Error when opening input audio" << std::endl;
        return false;
    }
    SDL_PauseAudioDevice(input, 1);
    SDL_ClearQueuedAudio(input);
    return true;
}

bool SDLAudioManager::SetupOutput(const char *deviceName)
{
    SDL_AudioSpec have;
    output = SDL_OpenAudioDevice(deviceName, 0, &AUDIO_SPEC, &have, 0);
    if (AUDIO_SPEC.format != have.format)
    {
        std::cout << "Error when opening output audio" << std::endl;
        return false;
    }
    SDL_PauseAudioDevice(output, 1);
    SDL_ClearQueuedAudio(output);
    return true;
}

Sint16 *SDLAudioManager::RecordAudio()
{
    Sint16 *data = new Sint16[AUDIO_BUF];
    auto size = SDL_DequeueAudio(input, data, AUDIO_BUF);

    if (size == 0)
        return nullptr;
    for (auto processor : inputProcessors)
    {
        processor->ProcessSound(data, AUDIO_BUF);
    }
    return data;
}

void SDLAudioManager::PlayAudio(Sint16 *data)
{

    if (data != NULL)
    {
        for (auto processor : outputProcessors)
        {
            processor->ProcessSound(data, AUDIO_BUF);
        }
        auto count = SDL_QueueAudio(output, data, AUDIO_BUF);
    }
}

void SDLAudioManager::Start()
{
    SDL_PauseAudioDevice(output, 0);
    SDL_PauseAudioDevice(input, 0);
    SDL_ClearQueuedAudio(input);
    SDL_ClearQueuedAudio(output);
}

void SDLAudioManager::Stop()
{
    SDL_PauseAudioDevice(output, 1);
    SDL_PauseAudioDevice(input, 1);
}