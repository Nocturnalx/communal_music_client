#pragma once

#include "SampleTypes.h"
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

        unsigned int m_drumSamples = 4;

        //for testing
        unsigned int m_stepsPlayed = 0;

        // Sample m_sampleArr[4] = {Sample("samples/default/kick.wav"), 
        //                     Sample("samples/default/snare.wav"),
        //                     Sample("samples/default/cymbal.wav"),
        //                     Sample("samples/default/hihat.wav")};
        
        DrumSample m_drumSampleArr[4] = {
        DrumSample("samples/amen1/kick.wav"), 
        DrumSample("samples/amen1/snare.wav"),
        DrumSample("samples/amen1/hihat.wav"),
        DrumSample("samples/amen1/cymbal.wav")
        };

        unsigned int m_instrumentSamples = 2;

        InstrumentSample m_instrumentSampleArr[2] = {
        InstrumentSample("samples/grand_piano/c5.wav"),
        InstrumentSample("samples/grand_piano/c5.wav")
        };

        Transport m_transport = Transport(m_bpm, m_drumSampleArr, m_drumSamples, m_instrumentSampleArr, m_instrumentSamples);

        controller_api m_controller = controller_api(&m_transport);

    public:

        bool eof = false;

        int samplesState = 0;

        Player(unsigned int sampleRate, unsigned int leng_seconds, unsigned int freq);
        ~Player();

        void populateSamples(short int * bufferPtr, unsigned long num);
};