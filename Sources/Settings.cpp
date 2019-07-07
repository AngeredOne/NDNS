#include "Settings.h"
#include "SDLAudioManager.h"
#include <iostream>

Settings::Settings()
{
    SDL_zero(config);
    config.freq = 44100;
    config.format = AUDIO_S16;
    config.channels = 1;
    config.samples = 1024;
    config.callback = nullptr;

    SetupFromConsole();
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