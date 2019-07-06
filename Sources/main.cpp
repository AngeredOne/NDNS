#include <iostream>
#include "VoiceClient.h"

int main(int, char **)
{
    //NO DISCORD NO SKYPE
    std::cout << "<---===:::::::::::::::::::::===--->\n";
    std::cout << "<---===       N_D_N_S       ===--->\n";
    std::cout << "<---=== NO DISCORD NO SKYPE ===--->\n";
    std::cout << "<---===:::::::::::::::::::::===--->\n";



    std::cout << "Enter IP: ";
    std::string remoteIP;
    getline(std::cin, remoteIP);

    SDLAudioManager::Get();
    std::cout << "Select devices" << std::endl;

    std::cout << "OUTPUT:" << std::endl;
    for (int i = 0; i < SDL_GetNumAudioDevices(0); ++i)
        std::cout << i << ":" << SDL_GetAudioDeviceName(i, 0) << std::endl;

    std::cout << std::endl;
    int index;
    std::cin >> index;
    auto outputDeviceName = SDL_GetAudioDeviceName(index, 0);

    std::cout << "INPUT:" << std::endl;
    for (int i = 0; i < SDL_GetNumAudioDevices(1); ++i)
        std::cout << i << ":" << SDL_GetAudioDeviceName(i, 1) << std::endl;

    std::cout << std::endl;
    std::cin >> index;
    auto inputDeviceName = SDL_GetAudioDeviceName(index, 1);

    SDL_AudioSpec config;
    SDL_zero(config);
    config.freq = 44100;
    config.format = AUDIO_S16;
    config.channels = 1;
    config.samples = 1024;
    config.callback = nullptr;

    if (SDLAudioManager::Get().SetupInput(inputDeviceName, config))
        std::cout << "INPUT OK " << inputDeviceName << std::endl;
    else
        std::cout << "INPUT ERROR" << std::endl;
    if (SDLAudioManager::Get().SetupOutput(outputDeviceName, config))
        std::cout << "OUTPUT OK " << outputDeviceName << std::endl;
    else
        std::cout << "OUTPUT ERROR" << std::endl;

    VoiceClient *vclient = new VoiceClient(remoteIP);

    while (true)
    {
        int buf = 1024;
        Uint8 *data = SDLAudioManager::Get().RecordAudio(buf);
        vclient->SendAudio(data, buf);
        //delete data;
    }
}
