
#include "Transport.h"
#include <cmath>
#include <ostream>
#include <iostream>

//Transport definitions
Transport::Transport(unsigned int bpm, DrumSample * drumSampleArr, unsigned int drumSamples, InstrumentSample * instrumentSampleArr, unsigned int instrumentSamples){

    m_bpm = bpm;
    double bps = (double)bpm / 60.0;
    m_divisionSec = 1.0 /(bps * 2); //setting division used by transport - *2 here means there is 2 sample calls per actual bpm beat

    // std::cout << "bpm: " << m_bpm << std::endl;
    // std::cout << "division in seconds: " << m_divisionSec << std::endl;

    drumSamplesCount = drumSamples;
    m_drumSampleArr = drumSampleArr;

    instrumentSamplesCount = instrumentSamples;
    m_instrumentSampleArr = instrumentSampleArr;

    totalChannelCount = drumSamplesCount + instrumentSamplesCount;

    std::cout << "total channel count: " << totalChannelCount << std::endl;

    //zero transport buffer data so its not reading random memory
    for (int i = 0; i < audioDataLeng; i++){
        audioData[i] = 0;
    }
}

Transport::~Transport(){        
    if(m_running){

        m_running = false;

        m_transportThread.join();
    }
}

//start transport loop in a new thread
int Transport::startTransport(){

    m_running = true;
    m_transportThread = std::thread(&Transport::transportLoop, this);

    return 0;
}

void Transport::transportLoop(){

    m_lastStepTime = high_resolution_clock::now();
    m_lastIncTime = m_lastStepTime;

    // double secondsPassed = 0;

    while(m_running){

        //get current time and see if there has been enough time since last pass to inc step and inc point in transport buffer
        high_resolution_clock::time_point currentTime = high_resolution_clock::now();

        //if time since last step fired
        duration<double> timeSinceStep = currentTime - m_lastStepTime;

        if(timeSinceStep.count() >= m_divisionSec){
            //put audio into transport buffer

            std::lock_guard<std::mutex> lock(m_stepNumMutex);

            m_currentStep = (m_currentStep + 1) % stepCount; //inc step

            unsigned int currentStep = m_currentStep; //copy val to free the lock

            lock.~lock_guard();

            std::cout << "step: " << currentStep << std::endl;

            //copy the new sequence over before playing step 0
            if (currentStep == 0){
                for (int c = 0; c < totalChannelCount; c++){
                    for (int s = 0; s < stepCount; s++){
                        //update channel states and note, even if its a drum ~ drum note will always be "60"
                        m_stepArray[s].setChannelState(c, m_nextSequence[s].getChannelState(c));
                        m_stepArray[s].setChannelNote(c, m_nextSequence[s].getChannelNote(c));
                    }
                }
            }

            //incresease write pointer
            unsigned int samplesSinceInc = std::ceil(timeSinceStep.count() * m_sampleRate);
            //need to catch drift and account here if round starts adding extra samples
            m_drift += (float)samplesSinceInc - (timeSinceStep.count() * m_sampleRate);

            if (m_drift > 1.0){
                --samplesSinceInc;
                m_drift = 0;
            }

            // std::cout << "drift: " << m_drift << std::endl;
            // std::cout << "samplesSinceInc: " << samplesSinceInc << std::endl;

            m_writePointer = (m_writePointer + samplesSinceInc) % audioDataLeng;

            // std::cout << "diff: " << m_writePointer - readPointer << std::endl;

            // check channel vals for this step and corresponding samples are activated
            for (int c = 0; c < drumSamplesCount; c++){
                if(m_stepArray[currentStep].getChannelState(c) == true){
                    //put audio into transport array from this point
                    m_drumSampleArr[c].playSample(audioData, m_writePointer, audioDataLeng);
                }
            }

            for (int c = drumSamplesCount; c < totalChannelCount; c++){
                if(m_stepArray[currentStep].getChannelState(c) == true){
                    m_instrumentSampleArr[c - drumSamplesCount].playSample(audioData, m_writePointer, audioDataLeng, m_stepArray[currentStep].getChannelNote(c));
                }
            }

            // need to think more about timings in the future
            // solid start time value to anchor from so there isn't drift? 
            // how would you prevent double type prescision loss over time?
            m_lastStepTime = currentTime; 
        }
    }
}

void Transport::setNextSequenceStates(bool (*steps)[16]){

    for (int c = 0; c < totalChannelCount; c++){
        for (int s = 0; s < stepCount; s++){
            m_nextSequence[s].setChannelState(c, steps[c][s]);
        }
    }
}

void Transport::setNextSequenceNotes(int (*steps)[16]){

    for (int c = drumSamplesCount; c < totalChannelCount; c++){
        for (int s = 0; s < stepCount; s++){
            // std::cout << steps[c - drumSamplesCount][s] << std::endl;
            m_nextSequence[s].setChannelNote(c, steps[c][s]);
        }
    }
}

unsigned int Transport::currentStepNumber(){

    std::lock_guard<std::mutex> lock(m_stepNumMutex);

    return m_currentStep;
}


//Step definitions
bool Step::getChannelState(unsigned int channel){

    std::lock_guard lock(m_channelArrMutex);
    
    return m_channelStates[channel];
}

void Step::setChannelState(unsigned int channel, bool val){

    std::lock_guard lock(m_channelArrMutex);

    m_channelStates[channel] = val;    
}

unsigned int Step::getChannelNote(unsigned int channel){

    std::lock_guard lock(m_channelArrMutex);
    
    return m_channelNotes[channel];
}

void Step::setChannelNote(unsigned int channel, unsigned int val){

    std::lock_guard lock(m_channelArrMutex);

    m_channelNotes[channel] = val;    
}