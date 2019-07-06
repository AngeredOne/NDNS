#include <iostream>
#include <SFML/Audio.hpp>
#include <unistd.h>


//с этой хуйней идея была такая - делаешь объект наследованный от этого - передаешь потом MicroTranslator и он все семплы с микро кидает туда
class ISoundInterprer {
    public: //у меня это билдилось я хз
    virtual void InterpretSamples(sf::Int16 *, std::size_t size ) = 0;
};
class AudioStream : public sf::SoundStream, public ISoundInterprer {
    public:
    bool Start();
    //вызывать этот метод чтобы проигрывать собственно звук
    virtual void InterpretSamples(sf::Int16 * samples, std::size_t size );
    sf::Int16 * samples;
    std::size_t size;
    
    private:
    virtual bool onGetData(Chunk & data);
    virtual void onSeek(sf::Time timeoffset){}
};


class MicroTranslator : public sf::SoundRecorder {
    public:
    ISoundInterprer * interpretator;
    void SetUp(ISoundInterprer * in) {
        interpretator = in;
        setProcessingInterval(sf::milliseconds(100));
    }
    virtual bool onProcessSamples(const sf::Int16 * samples, std::size_t sampleCount);
};