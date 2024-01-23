#pragma once

#include <math.h>
#include <iostream>
#include <memory>
#include <chrono>

#include "Sample.h"
#include "Transport.h"
#include "Network.h"

class Player{
    private:
        unsigned int m_sampleRate;
        unsigned int m_sampleSize = sizeof(short int);
        unsigned int m_lengSamps;
        unsigned int m_pointInAudio = 0;
        unsigned int m_pointInSin = 0;
        float m_frequency;
        unsigned int m_periodSamples;
        bool m_initialised = false;

        unsigned int m_bpm = 170;

        unsigned int m_currentStep = 0; // -1 being the imaginary previous step

        unsigned int m_channels = 4;

        // Sample m_sampleArr[4] = {Sample("samples/default/kick.wav"), 
        //                     Sample("samples/default/snare.wav"),
        //                     Sample("samples/default/cymbal.wav"),
        //                     Sample("samples/default/hihat.wav")};
        
        Sample m_sampleArr[4] = {Sample("samples/amen1/kick.wav"), 
                            Sample("samples/amen1/snare.wav"),
                            Sample("samples/amen1/hihat.wav"),
                            Sample("samples/amen1/cymbal.wav")};

        Transport m_transport = Transport(m_bpm, m_sampleArr, m_channels);

        controller_api m_controller = controller_api(&m_transport);

        //for testing
        unsigned int m_stepsPlayed = 0;

    public:

        bool eof = false;

        int samplesState = 0;

        Player(unsigned int sampleRate, unsigned int leng_seconds, unsigned int freq);
        ~Player();

        void populateSamples(short int * bufferPtr, unsigned long num);
};