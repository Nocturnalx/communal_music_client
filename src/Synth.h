#pragma once

struct synthData {
    
    unsigned int noteLength = 44100/2;

    bool playSin = false;
    unsigned int sinAttack = 44100/64;
    double sinGain = 1.0;

    bool playSaw = false;
    unsigned int sawAttack = 44100/64;
    double sawGain = 1.0;

    bool playTri = false;
    unsigned int triAttack = 44100/64;
    double triGain = 1.0;
};

class Synth{
    
    private:

        // unsigned int m_totalLength = 44100 / 2;
        // unsigned int m_attackSamps = 44100 / 64;

        int m_signalGain = 5000; //can be no higher than 0xFFFF(signed short int max)

        synthData m_synthData;

        void playSin(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, unsigned int periodSamples);
        void playSaw(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, unsigned int periodSamples);
        void playTri(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, unsigned int periodSamples);

    public:

        void play(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, int note);

        void init(synthData data);

};