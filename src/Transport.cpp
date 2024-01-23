
#include "Transport.h"

//Transport definitions
Transport::Transport(unsigned int bpm, Sample * sampleArr, unsigned int channels){

    m_bpm = bpm;
    double bps = (double)bpm / 60.0;
    m_divisionSec = 1.0 /(bps * 2);

    // std::cout << "bpm: " << m_bpm << std::endl;
    // std::cout << "division in seconds: " << m_divisionSec << std::endl;

    channelCount = channels;
    m_sampleArr = sampleArr;

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
                for (int c = 0; c < channelCount; c++){
                    for (int s = 0; s < stepCount; s++){
                        m_stepArray[s].setChannelState(c, m_nextSequence[s].getChannelState(c));
                    }
                }
            }

            unsigned int samplesSinceInc = std::round(timeSinceStep.count() * 44100);

            // std::cout << "samplesSinceInc: " << samplesSinceInc << std::endl;

            m_writePointer = (m_writePointer + samplesSinceInc) % audioDataLeng;

            // check channel vals for this step and corresponding samples are activated
            for (int c = 0; c < channelCount; c++){
                
                if(m_stepArray[currentStep].getChannelState(c) == true){
                    //put audio into transport array from this point
                    m_sampleArr[c].playSample(audioData, m_writePointer, audioDataLeng);
                }
            }
            
            // need to think more about timings in the future
            // solid start time value to anchor from so there isn't drift? 
            // how would you prevent double type prescision loss over time?
            m_lastStepTime = currentTime; 
        }
    }
}

void Transport::setNextSequence(bool (*steps)[16]){

    for (int c = 0; c < channelCount; c++){
        for (int s = 0; s < stepCount; s++){
            m_nextSequence[s].setChannelState(c, steps[c][s]);
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
    
    return m_channels[channel];
}

void Step::setChannelState(unsigned int channel, bool val){

    std::lock_guard lock(m_channelArrMutex);

    m_channels[channel] = val;    
}