#pragma once

#include <SDL2/SDL.h>
#include <math.h>

class Loudness {
    public:
    Loudness() {}
    Loudness(float _volume);
    //For pretty output
    void SetVolume(int percentage);
    int GetVolume() const;

    constexpr int MaxVolumeValue();
    //For computations
    float GetVolumeCoef() const;
    private:
    const float MAX_VOLUME = 1.5f;
    const float MIN_VOLUME = 0.001f;
    
    int volume = 50;

};

class Settings
{
public:
    Loudness input;
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