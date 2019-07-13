#pragma once
#include "SDLAudioManager.h"

class AbstractSoundProcessor {
    public:
    virtual void ProcessSound(Sint16* samples, int len)=0;
};

class Volume : public AbstractSoundProcessor {
    public:
    virtual void ProcessSound(Sint16* samples, int len);
};
