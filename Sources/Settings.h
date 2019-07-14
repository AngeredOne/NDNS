#pragma once
#include "NDNS.h"



class Loudness {
    public:
    Loudness() {}
    Loudness(float _volume);
    //VOLUME
    //Interface
    void SetVolume(int percentage);
    int GetVolume() const;
    constexpr int MaxVolumeValue();
    //Computations
    float GetVolumeCoef() const;
    //THRESHOLD
    //Interface
    void SetThreshold(int percentage);
    int GetThreshold() const;
    //Computations
    float GetThresholdCoef() const;
    private:

    const float MAX_VOLUME = 4.f;
    const float MIN_VOLUME = 0.01f;
    int volume = 1;

    const int MAX_THRESHOLD = 100;
    const int MIN_THRESHOLD = 0;
    int threshold = 15;

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