#pragma once

#include <thread>
#include <mutex>
#include <chrono>


#include "SampleTypes.h"

using namespace std::chrono;


// class DrumSample;

class Step {
    
    private:

        std::mutex m_channelArrMutex;

        //4 channels, this holds on/off values for each channel in this step
        bool m_channelStates[6] =     {false, false, false, false, false, false};
        unsigned int m_channelNotes[6] = {60,    60,    60,    60,    60,    70};

    public:

        unsigned int stepNum;

        void setChannelState(unsigned int channel, bool val);
        bool getChannelState(unsigned int channel);

        void setChannelNote(unsigned int channel, unsigned int val);
        unsigned int getChannelNote(unsigned int channel);
};

class Transport{

    private:

        unsigned int m_sampleRate = 44100;

        unsigned int m_pointInSequence;
        unsigned int m_bpm;
        // unsigned int m_lengthInSamples;
        // unsigned int m_sampleRate;

        double m_divisionSec;
        high_resolution_clock::time_point m_lastStepTime;

        bool m_running = false;
        std::thread m_transportThread;

        std::mutex m_stepNumMutex;
        unsigned int m_currentStep = 0;

        // std::mutex m_stepArrMutex;
        Step m_stepArray[16];
        Step m_nextSequence[16]; //step array of the next sequence, gets updated by the controller

        DrumSample * m_drumSampleArr;
        InstrumentSample * m_instrumentSampleArr;

        high_resolution_clock::time_point m_lastIncTime;

        float m_drift = 0;

        unsigned int m_writePointer = 10000; //higher this starts the more latency but also the more time you have for the samples to populate audio buffer 

        void transportLoop();

    public:

        Transport(unsigned int bpm, DrumSample * drumSampleArr, unsigned int drumSamples, InstrumentSample * instrumentSampleArr, unsigned int instrumentSamples);
        ~Transport();

        unsigned int readPointer = 0;

        unsigned int audioDataLeng = 384000; 
        short int audioData[384000];

        unsigned int stepCount = 16;
        //these get set in constructor
        unsigned int drumSamplesCount; //drum channel count
        unsigned int instrumentSamplesCount;
        unsigned int totalChannelCount;

        //launch separate thread which increments point in sequence according to real time clock
        int startTransport();

        void setNextSequenceStates(bool (*steps)[16]);
        void setNextSequenceNotes(int (*steps)[16]);

        //returns step struct for given step, meaning state of all channels for "this step"
        // Step getStep(unsigned int step);
        // void setSteps(Step * stepsIn);

        // void stopTransport();

        unsigned int currentStepNumber();
};
