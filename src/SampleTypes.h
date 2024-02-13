#pragma once

#include "Sample.h"

class DrumSample : public Sample{

    public:
        DrumSample(std::string path);
        
        void playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng) override;
};

class InstrumentSample : public Sample{

    private:
        int attackSamps = 44100 / 32;

    public:
        InstrumentSample(std::string path);
    
        void playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, int note) override;
};