#include <iostream>
#include <SDL2/SDL.h>
#include "VoiceClient.h"
#include "Settings.h"
#include "NDNS.h"

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

   NDNS::Get().Start();
   while(true);
}
