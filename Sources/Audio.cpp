#include "Audio.h"

bool AudioStream::Start()
{
    if (sf::SoundBufferRecorder::isAvailable())
    {
        initialize(1, 44100);
        return true;
    }
    return false;
}

void AudioStream::InterpretSamples(sf::Int16 *samples, std::size_t size)
{
    this->samples = samples;
    this->size = size;
    if (getStatus() != Playing)
    {
        play();
    }
}

bool AudioStream::onGetData(Chunk &data)
{
    if (size > 0)
    {
        data.samples = samples;
        data.sampleCount = size;
        samples == nullptr;
        size = 0;
    }
    return true;
}

bool MicroTranslator::onProcessSamples(const sf::Int16 *samples, std::size_t sampleCount)
{
    interpretator->InterpretSamples((sf::Int16 *)samples, sampleCount);
    return true;
}