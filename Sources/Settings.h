#pragma once

#include <SDL2/SDL.h>
#include <math.h>
#include <memory>



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
    const float MAX_VOLUME = 4.f;
    const float MIN_VOLUME = 0.01f;
    
    int volume = 1;

};

class Settings
{
public:
    std::shared_ptr<Loudness> input = std::make_shared<Loudness>(100);
    std::shared_ptr<Loudness> output = std::make_shared<Loudness>(100);
    static Settings &Get()
    {
        static Settings instance;
        return instance;
    }
    void SetupFromConsole();
private:
    Settings();
    Settings(Settings const &) = delete;
    Settings &operator=(Settings const &) = delete;
    SDL_AudioSpec config;
};