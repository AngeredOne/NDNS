#pragma once
#include "NDNS.h"
class AbstractSoundProcessor {
    public:
    virtual void ProcessSound(Sint16* samples, int len)=0;
};


class Volume : public AbstractSoundProcessor {
    public:
    Volume(std::function<float()> func) : GetVolume(func) {}
    virtual void ProcessSound(Sint16* samples, int len);
    private: 
    std::function<float()> GetVolume;
};

class Threshold : public AbstractSoundProcessor {
    public:
    Threshold(std::function<float()> func) : GetThreshold(func) {}
    virtual void ProcessSound(Sint16* samples, int len);
    private:
    std::function<float()> GetThreshold;
};