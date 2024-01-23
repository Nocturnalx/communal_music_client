#include "Player.h"

Player::Player(unsigned int sampleRate, unsigned int leng_samps, unsigned int freq){

    m_sampleRate = sampleRate;
    m_lengSamps = leng_samps * sampleRate;
    m_periodSamples = sampleRate / freq;

    //for samples in sampArr
    for (int i = 0; i < m_channels; i++){
        samplesState += m_sampleArr[i].loadSample();
    }

    //give sample pointers to transport

    if (m_controller.startController() != 0){
        std::cout << "error starting controller thread\n";
    }

    if (m_transport.startTransport() != 0){
        std::cout << "error starting transport thread\n";
    }

    m_initialised = true;
}

Player::~Player(){
    
}

void Player::populateSamples(short int * bufferPtr, unsigned long num){

    if (!m_initialised) return; //dont allow processing if not initialised

    //get audio from the transport buffer, increment read pointer
    for (int i = 0; i < num; i++){

        //reader and writer should theoretically never access the same 
        //point in the transport data array due to the offset
        //so mutex would just slow it down
        bufferPtr[i] = m_transport.audioData[m_transport.readPointer]; 

        m_transport.audioData[m_transport.readPointer] = 0; //reset sample once read

        m_transport.readPointer = (m_transport.readPointer + 1) % m_transport.audioDataLeng;
    }

    //for ensuring playback doesnt last forever - testing 
    unsigned int newStepNum = m_transport.currentStepNumber();
    if (newStepNum != m_currentStep){
        m_stepsPlayed++;
        m_currentStep = newStepNum;
    }

    if(m_stepsPlayed > (32 * 16)){
        eof = true;
    }
}