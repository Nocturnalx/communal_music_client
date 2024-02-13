#include "Network.h"
#include <cstddef>
#include <iostream>

controller_api::controller_api(Transport * transportPointer){
    
    m_transport = transportPointer;

    //request options
    m_request.setOpt(new curlpp::options::Url("https://utladal.com/audio/steps"));

    std::list<std::string> headers;
    headers.push_back("Content-Type: application/json");
    headers.push_back("secret: velvet-undergound");
    m_request.setOpt(new curlpp::options::HttpHeader(headers));

    m_request.setOpt(new curlpp::options::CustomRequest{"GET"});
}

controller_api::~controller_api(){
    
    if (m_running){
        m_running = false;

        m_controllerThread.join();
    }
}

//function that calls looper to start
int controller_api::startController(){

    //start loop in new thread

    m_running = true;
    m_controllerThread = std::thread(&controller_api::controllerLoop, this);

    return 0;
}

void controller_api::controllerLoop(){

    bool updatedSequence = false;

    while(m_running){
        //when m_transport gets to step 4, call server for next update
        unsigned int newStepNum = m_transport->currentStepNumber(); //lock immediately goes out of scope so wont be too blocking

        if (newStepNum == 4 && !updatedSequence){

            //make network call, this will populate m_newSequence
            updateSequence();

            m_transport->setNextSequenceStates(m_newSequenceStates);
            m_transport->setNextSequenceNotes(m_newSequenceNotes);

            updatedSequence = true;
        }

        if (updatedSequence && newStepNum != 4){
            updatedSequence = false;
        }
    }
}

void controller_api::updateSequence(){
    //make network call

    std::stringstream responseStream;
    curlpp::options::WriteStream ws(&responseStream);
    m_request.setOpt(ws);

    m_request.perform();

    nlohmann::json jsonResponse = nlohmann::json::parse(responseStream.str());

    // std::cout << "Parsed JSON: " << jsonResponse.dump(2) << std::endl;

    if (jsonResponse.contains("stateArray") == false || jsonResponse["stateArray"].is_array() == false || jsonResponse["stateArray"] == NULL){
        std::cout << "state array not found in controller response\n";
        return;
    }

    if (jsonResponse.contains("noteArray") == false || jsonResponse["noteArray"].is_array() == false || jsonResponse["noteArray"] == NULL){
        std::cout << "note array not found in controller response\n";
        return;
    }

    auto jsonStateArray = jsonResponse["stateArray"];

    //make sure the array coming is is the right size
    if (jsonStateArray.size() != m_transport->totalChannelCount || jsonStateArray[0].size() != m_transport->stepCount) {

        std::cout << "incoming channels: " << jsonStateArray.size() << std::endl;
        std::cout << "incoming steps: " << jsonStateArray[0].size() << std::endl;
        
        std::cout << "controll array size incompatible with transport step array size!\n";
        return;
    }

    auto jsonNoteArray = jsonResponse["noteArray"];

    if (jsonNoteArray.size() != m_transport->synthsCount || jsonNoteArray[0].size() != m_transport->stepCount){

        std::cout << "incoming instrument channels: " << jsonNoteArray.size() << std::endl;
        std::cout << "incoming steps: " << jsonNoteArray[0].size() << std::endl;
        
        std::cout << "instrument controll array size incompatible with transport step array size!\n";
        return;
    }

    // std::cout << "Parsed JSON: " << jsonNoteArray.dump(1) << std::endl;

    //do total for all samples and set states (change drumSamplesCount to totalCount when website is updated to 6 channels)
    for (int c = 0; c < m_transport->totalChannelCount; c++){
        for (int s = 0; s < m_transport->stepCount; s++){
            
            m_newSequenceStates[c][s] = (jsonStateArray[c][s].get<int>() == 1); //get bool from int 1 or 0

            if (c >= m_transport->drumSamplesCount){
                m_newSequenceNotes[c][s] = jsonNoteArray[c - m_transport->drumSamplesCount][s].get<int>();
            }
        }
    }
}