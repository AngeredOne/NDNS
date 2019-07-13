#include "SoundProcessing.h"
#include "Settings.h"
#include <iostream>
#include <math.h>
void Volume::ProcessSound(Sint16 *samples, int len)
{
    float volume_setting = Settings::Get().input.GetVolumeCoef();
    Sint16 *sample = &samples[0];
    
    for (int i = 0; i < len; ++i)
    {
        *sample *= volume_setting;
        sample++;
        std::cout << *sample << " ";
    }
    std::cout << std::endl << std::endl;
}
