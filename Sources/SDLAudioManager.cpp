#include "SDLAudioManager.h"
#include "SoundProcessing.h"
#include "Settings.h"
#include "SettingsFields.h"


void SDLAudioManager::InitProcessors() {
  
  auto controller = Settings::Get().GetField(S_VOLUME_IN);
  inputProcessors.push_back(std::make_shared<Volume>(std::bind(&VolumeControl::GetVolume, static_cast<VolumeControl*>(controller.get()))));

  controller = Settings::Get().GetField(S_THRESHOLD_IN);
  inputProcessors.push_back(std::make_shared<Threshold>(std::bind(&ThresholdControl::GetThreshold, static_cast<ThresholdControl*>(controller.get()))));

  controller = Settings::Get().GetField(S_VOLUME_OUT);
  outputProcessors.push_back(std::make_shared<Volume>(std::bind(&VolumeControl::GetVolume, static_cast<VolumeControl*>(controller.get()))));


  
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
    return true;
}

bool SDLAudioManager::SetupOutput(const char *deviceName)
{
    SDL_AudioSpec have;
    output = SDL_OpenAudioDevice(deviceName, 0, &AUDIO_SPEC, &have, 0);
    if (AUDIO_SPEC.format != have.format)
    {
        std::cout << "Error when opening input audio" << std::endl;
        return false;
    }
    SDL_PauseAudioDevice(output, 1);
    return true;
}

Sint16 *SDLAudioManager::RecordAudio(int len)
{
    Sint16 *data = new Sint16[len];
    auto size = SDL_DequeueAudio(input, data, len);
    SDL_Delay(10);
    if (size == 0)
        return nullptr;
    for (auto processor : inputProcessors) {
        processor->ProcessSound(data, len);
    }
    return data;
}

void SDLAudioManager::PlayAudio(Sint16 *data, int len)
{

    if (data != NULL)
    {
        for (auto processor : outputProcessors) {
            processor->ProcessSound(data, len);
        }
        auto count = SDL_QueueAudio(output, data, len);
    }
}

void SDLAudioManager::Start() {
    SDL_ClearQueuedAudio(input);
    SDL_ClearQueuedAudio(output);
    SDL_PauseAudioDevice(output, 0);
    SDL_PauseAudioDevice(input, 0);
}

void SDLAudioManager::Stop() {
    SDL_PauseAudioDevice(output, 1);
    SDL_PauseAudioDevice(input, 1);
}