#include "SampleTypes.h"
#include "Sample.h"

#include <iostream>
#include <cmath>

DrumSample::DrumSample(std::string path) : Sample(path){}

void DrumSample::playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng){

    m_pointInAudio = 0;

    for (int i = 0; i < m_lengSamps; i++){

        transportAudioBuff[writePointer] += m_audioData[m_pointInAudio] * m_gain;

        writePointer = (writePointer + 1) % bufferLeng;

        m_pointInAudio++;
    }
}

InstrumentSample::InstrumentSample(std::string path) : Sample(path){}

void InstrumentSample::playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, int note){
    
    m_pointInAudio = 0;

    double a440Freq = 440.0;
    double freq = a440Freq * pow(2.0, (note - 69)/12.0);
    int periodSamples = 44100/freq;

    // std::cout << note << std::endl;
    // std::cout << freq << std::endl;
    // std::cout << periodSamples << std::endl;

    //placeholder
    for (int i = 0; i < m_lengSamps /2; i++){

        short int sinSample = 5000 * sin((2*M_PI)*writePointer/periodSamples);

        double envGain;
        if (i < attackSamps){
            envGain = (double)i / (double)attackSamps;
        } else{
            envGain = ((((double)m_lengSamps / 2) - attackSamps) - (i - attackSamps)) / (((double)m_lengSamps / 2) - attackSamps);
        }

        transportAudioBuff[writePointer] += sinSample * envGain;

        writePointer = (writePointer + 1) % bufferLeng;

        m_pointInAudio++;
    }
}