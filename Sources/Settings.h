#pragma once

#include <SDL2/SDL.h>

class Settings
{
    public:
    Settings();
    private:
    void SetupFromConsole();

    SDL_AudioSpec config;


};