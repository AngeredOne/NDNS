#include <iostream>
#include <SDL2/SDL.h>
#include "VoiceClient.h"
#include "Settings.h"
#include "NDNS.h"

int main(int, char **)
{

   NDNS::Get().Start();
   while(true);
}
