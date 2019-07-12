#pragma once

#include <SDL2/SDL.h>

class Settings
{
public:
    static Settings &Get()
    {
        static Settings instance;
        return instance;
    }

private:
    Settings();
    Settings(Settings const &) = delete;
    Settings &operator=(Settings const &) = delete;
    void SetupFromConsole();
    SDL_AudioSpec config;
};