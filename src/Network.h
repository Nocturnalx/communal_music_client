#include <thread>
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <nlohmann/json.hpp>

#include "Transport.h"

class controller_api
{
    private:
        
        bool m_running;
        std::thread m_controllerThread;

        curlpp::Cleanup m_curlppCleaner;
        curlpp::Easy m_request;

        Transport * m_transport;

        bool m_newSequence[4][16];

        void controllerLoop();

        void updateSequence();

    public:
        controller_api(Transport * transportPointer);
        ~controller_api();

        int startController();
};