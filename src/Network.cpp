#include "Network.h"

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

            m_transport->setNextSequence(m_newSequence);

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

    if (jsonResponse.contains("stepArray") && jsonResponse["stepArray"].is_array()){
        
        auto jsonArray = jsonResponse["stepArray"];

        // std::cout << "Parsed JSON: " << jsonArray.dump(2) << std::endl;

        //make sure the array coming is is the right size
        if (jsonArray.size() != m_transport->channelCount || jsonArray[0].size() != m_transport->stepCount) {

            std::cout << "incoming channels: " << jsonArray.size() << std::endl;
            std::cout << "incoming steps: " << jsonArray[0].size() << std::endl;
            
            std::cout << "controll array size incompatible with transport step array size!\n";
            return;
        }

        for (int c = 0; c < m_transport->channelCount; c++){
            for (int s = 0; s < m_transport->stepCount; s++){
                m_newSequence[c][s] = (jsonArray[c][s].get<int>() == 1); //get bool from int 1 or 0
            }
        }
    }
}