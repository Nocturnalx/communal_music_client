#include "Synth.h"

#include <cstddef>
#include <cstdlib>
#include <math.h>
#include <time.h>

void Synth::init(synthData data){
    m_synthData = data;
}

void Synth::play(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, int note){

    srand(time(NULL));

    double a440Freq = 440.0;
    double freq = a440Freq * pow(2.0, (note - 69)/12.0);
    unsigned int periodSamples = 44100/freq;

    if (m_synthData.playSin){
        playSin(transportAudioBuff, writePointer, bufferLeng, periodSamples);
    }

    if (m_synthData.playSaw){
        playSaw(transportAudioBuff, writePointer, bufferLeng, periodSamples);
    }

    if (m_synthData.playTri){
        playTri(transportAudioBuff, writePointer, bufferLeng, periodSamples);
    }
}

void Synth::playSin(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, unsigned int periodSamples){
    
    unsigned int pointInWave = 0;

    int range = m_synthData.noteLength / 2;
    int randInt = (rand() % range) - (range / 2);
    int leng = m_synthData.noteLength + randInt;

    for (int i = 0; i < leng; i++){
        
        short int sinSample = m_signalGain * sin((2*M_PI)*pointInWave/periodSamples);

        pointInWave = (pointInWave + 1) % periodSamples;

        double envGain = 1;
        if (i < m_synthData.sinAttack){
            envGain = (double)i / (double)m_synthData.sinAttack;
        } else {
            envGain = (double)((leng - m_synthData.sinAttack) - (i - m_synthData.sinAttack)) / (leng - m_synthData.sinAttack);
        }

        transportAudioBuff[writePointer] += sinSample * envGain * m_synthData.sinGain;
        writePointer = (writePointer + 1) % bufferLeng;
    }
}

void Synth::playSaw(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, unsigned int periodSamples){

    unsigned int pointInWave = 0;

    int range = m_synthData.noteLength / 2;
    int randInt = (rand() % range) - (range / 2);
    int leng = m_synthData.noteLength + randInt;

    for (int i = 0; i < leng; i++){

        double phase = (2*M_PI)*pointInWave/periodSamples;

        short int sawSample = m_signalGain * ((1.0/M_PI) * (phase - 1));

        pointInWave = (pointInWave + 1) % periodSamples;

        double envGain = 1;
        if (i < m_synthData.sawAttack){
            envGain = (double)i / (double)m_synthData.sawAttack;
        } else {
            envGain = (double)((leng - m_synthData.sawAttack) - (i - m_synthData.sawAttack)) / (leng - m_synthData.sawAttack);
        }

        transportAudioBuff[writePointer] += sawSample * envGain * m_synthData.sawGain;
        writePointer = (writePointer + 1) % bufferLeng;
    }
}

void Synth::playTri(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, unsigned int periodSamples){

    unsigned int pointInWave = 0;

    int range = m_synthData.noteLength / 2;
    int randInt = (rand() % range) - (range / 2);
    int leng = m_synthData.noteLength + randInt;

    for (int i = 0; i < leng; i++){

        double phase = (2*M_PI)*pointInWave/periodSamples;

        short int triSample = m_signalGain * (phase < M_PI ? -1.0 + (2.0 / M_PI) * phase :  3.0 - (2.0 / M_PI) * phase);

        pointInWave = (pointInWave + 1) % periodSamples;

        double envGain = 1;
        if (i < m_synthData.triAttack){
            envGain = (double)i / (double)m_synthData.triAttack;
        } else {
            envGain = (double)((leng - m_synthData.triAttack) - (i - m_synthData.triAttack)) / (leng - m_synthData.triAttack);
        }

        transportAudioBuff[writePointer] += triSample * envGain * m_synthData.triGain;
        writePointer = (writePointer + 1) % bufferLeng;
    }
}