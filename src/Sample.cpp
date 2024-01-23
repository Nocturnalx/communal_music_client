#include "Sample.h"

Sample::Sample(std::string path){

    m_filePath = path;

    m_file = fopen(m_filePath.data(), "r");
}

Sample::~Sample(){
    fclose(m_file);

    if (m_dataTagFound){
        delete [] m_audioData;
    }
}

int Sample::loadSample(){

    //read untill the 'data' tag is found, then read data size

    char check_buff[1];
    char data_buff[4];

    //checking for data tag
    for (int i = 0; i < 1500; i++){
        fread(check_buff, sizeof(char), 1, m_file);
        // fwrite(check_buff, 1, sizeof(char), outfile);

        for (int i = 0; i < 3; i++){
            data_buff[i] = data_buff[i + 1];
        }

        data_buff[3] = check_buff[0];

        //there must be a better way to do this?????
        if (data_buff[3] == 'a' && data_buff[2] == 't' && data_buff[1] == 'a' && data_buff[0] == 'd'){
            m_dataTagFound = true;
            break;
        }
    }

    if (m_dataTagFound){
        //read next 2 bytes which is data size
        fread(&m_infileSize, 1, sizeof(int), m_file);
        m_lengSamps = m_infileSize / sizeof(short int);

        std::cout << "audio leng in samps: " << m_lengSamps << std::endl;

        //create 
        m_audioData = new short int [m_lengSamps];

        fread(m_audioData, sizeof(short int), m_lengSamps, m_file);

        return 0;
    } else {
        std::cout << "err no data tag found for wav sample." << std::endl;
        
        return 1;
    }
}

void Sample::playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int audioDataLeng){

    m_pointInAudio = 0;

    for (int i = 0; i < m_lengSamps; i++){

        transportAudioBuff[writePointer] += m_audioData[m_pointInAudio];

        writePointer = (writePointer + 1) % audioDataLeng; //192000 is transport audio buff size

        m_pointInAudio++;
    }
}