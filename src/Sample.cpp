#include "Sample.h"
#include <filesystem>
#include <iostream>
#include <ostream>

Sample::Sample(std::string path){

    m_filePath = path;
}

Sample::~Sample(){
    if (m_fileExists){

        fclose(m_file);

        //data tag found implies m_audioData was initialised
        if (m_dataTagFound){
            delete [] m_audioData;

            std::cout << "freeing " << m_filePath << std::endl;
        }
    }
}

int Sample::loadSample(){

    //check file exists before reading
    if (!std::filesystem::exists(m_filePath)){
        std::cout << m_filePath << " does not exist" << std::endl;
        return 1;
    }

    //moved past check so file exists
    m_fileExists = true;

    //read untill the 'data' tag is found, then read data size
    char check_buff[1];
    char data_buff[4];

    m_file = fopen(m_filePath.data(), "r");

    //checking for data tag
    for (int i = 0; i < 1500; i++){
        fread(check_buff, sizeof(char), 1, m_file);
        // fwrite(check_buff, 1, sizeof(char), outfile);

        for (int n = 0; n < 3; n++){
            data_buff[n] = data_buff[n + 1];
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

        std::cout << m_filePath << " - leng in samps: " << m_lengSamps << std::endl;

        //create 
        m_audioData = new short int [m_lengSamps];

        fread(m_audioData, sizeof(short int), m_lengSamps, m_file);

        return 0;
    } else {
        std::cout << "err no data tag found for wav sample: " << m_filePath << std::endl;
        
        return 1;
    }
}

void Sample::playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng){
    std::cout << "Error, sample loaded using base sample class instead of type classes\n";
}

void Sample::playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, int note){
    std::cout << "Error, sample loaded using base sample class instead of type classes\n";
}