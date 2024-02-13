#pragma once

#include "SampleTypes.h"
#include "Synth.h"
#include "Transport.h"
#include "Network.h"

class Player{
    private:
        unsigned int m_sampleRate;
        unsigned int m_sampleSize = sizeof(short int);

        bool m_initialised = false;

        unsigned int m_bpm = 170;

        unsigned int m_currentStep = 0; // -1 being the imaginary previous step

        //for testing
        unsigned int m_stepsPlayed = 0;

        // unsigned int m_instrumentSamples = 2;
        
        // InstrumentSample m_instrumentSampleArr[2] = {
        //     InstrumentSample("samples/grand_piano/c5.wav"),
        //     InstrumentSample("samples/grand_piano/c5.wav")
        // };
        
        unsigned int m_drumSamples = 4;
        DrumSample m_drumSampleArr[4] = {
            DrumSample("samples/amen1/kick.wav"), 
            DrumSample("samples/amen1/snare.wav"),
            DrumSample("samples/amen1/hihat.wav"),
            DrumSample("samples/amen1/cymbal.wav")
        };

        unsigned int m_synths = 2;
        Synth m_synthArr[2] = {
            Synth(), 
            Synth()
        };
        Transport m_transport = Transport(m_bpm, m_drumSampleArr, m_drumSamples, m_synthArr, m_synths);

        controller_api m_controller = controller_api(&m_transport);

    public:

        bool eof = false;

        int samplesState = 0;

        Player(unsigned int sampleRate, unsigned int leng_seconds, unsigned int freq);
        ~Player();

        void populateSamples(short int * bufferPtr, unsigned long num);
};