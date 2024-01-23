#pragma once

#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>
#include <math.h>

#include "Sample.h"

using namespace std::chrono;


class Sample;

class Step {
    
    private:    

        std::mutex m_channelArrMutex;

        bool m_channels[4] = {false, false, false, false}; //4 channels, this holds on/off values for each channel in this step

    public:

        unsigned int stepNum;

        void setChannelState(unsigned int channel, bool val);

        bool getChannelState(unsigned int channel);
};

class Transport{

    private:

        unsigned int m_pointInSequence;
        unsigned int m_bpm;
        // unsigned int m_lengthInSamples;
        // unsigned int m_sampleRate;

        double m_divisionSec;
        high_resolution_clock::time_point m_lastStepTime;

        bool m_running;
        std::thread m_transportThread;

        std::mutex m_stepNumMutex;
        unsigned int m_currentStep = 0;

        // std::mutex m_stepArrMutex;
        Step m_stepArray[16]; //8 steps
        Step m_nextSequence[16]; //step array of the next sequence, gets updated by the controller

        Sample * m_sampleArr;

        high_resolution_clock::time_point m_lastIncTime;

        unsigned int m_writePointer = 10000; //higher this starts the more latency but also the more time you have for the samples to populate audio buffer 

        void transportLoop();

    public:

        Transport(unsigned int bpm, Sample * sampleArr, unsigned int channels);
        ~Transport();

        unsigned int readPointer = 0;

        unsigned int audioDataLeng = 384000; 
        short int audioData[384000];

        unsigned int stepCount = 16;
        unsigned int channelCount; //set in constructor, relative to the ammount of samples in array

        //launch separate thread which increments point in sequence according to real time clock
        int startTransport();

        void setNextSequence(bool (*steps)[16]);

        //returns step struct for given step, meaning state of all channels for "this step"
        // Step getStep(unsigned int step);
        // void setSteps(Step * stepsIn);

        // void stopTransport();

        unsigned int currentStepNumber();
};
