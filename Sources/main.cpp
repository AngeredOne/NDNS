#include <iostream>
#include <SDL2/SDL.h>
#include "VoiceClient.h"
#include "Settings.h"

void Record(VoiceClient *vclient)
{
    SDLAudioManager::Get().StartRecord();
    while (true)
    {
        int buf = 1024;
        Sint16 *data = SDLAudioManager::Get().RecordAudio(buf);
        vclient->SendAudio((Sint8*) data, buf);
        delete data;
    }
}

int main(int, char **)
{
    //NO DISCORD NO SKYPE
    std::cout << "<---===:::::::::::::::::::::===--->\n";
    std::cout << "<---===       N_D_N_S       ===--->\n";
    std::cout << "<---=== NO DISCORD NO SKYPE ===--->\n";
    std::cout << "<---===:::::::::::::::::::::===--->\n";

    SDL_Init(SDL_INIT_AUDIO);
    SDL_AudioInit(SDL_GetAudioDriver(0));
    SDLAudioManager::Get();
    Settings::Get();

    std::cout << "Enter IP: ";
    std::string remoteIP;
    getline(std::cin, remoteIP);
    VoiceClient *vclient = new VoiceClient(remoteIP);

    std::thread *recordThread = new std::thread(&Record, vclient);
    //Record(vclient);
    while (true)
    {
        std::string message;
        getline(std::cin, message);
        vclient->SendMessage(message);
    }
    delete recordThread;
}
