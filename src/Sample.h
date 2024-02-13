#pragma once
#include <fstream>
#include <string.h>
#include <mutex>
#include <chrono>

using namespace std::chrono;

enum openType{
    FILE_READ,
    FILE_WRITE
};

typedef struct header_file
{
    char chunk_id[4];
    int chunk_size;
    char format[4]; //16 bytes

    char subchunk1_id[4];
    int subchunk1_size;
    short int audio_format;
    short int num_channels;
    int sample_rate;			// sample_rate denotes the sampling rate.
    int byte_rate;
    short int block_align;
    short int bits_per_sample; //36 bytes
} header;

typedef struct header_file* header_p;

class Sample{

    protected: 

        std::string m_filePath;
        FILE * m_file;
        bool m_fileExists = false;

        int m_infileSize;
        int m_bytesRead = 0;

        //for file read
        bool m_dataTagFound = false;

        unsigned int m_lengSamps = 0; // = 0 means it wont break when forget to load sample, but also wont show error
        short int * m_audioData;
        int m_pointInAudio = 0;

        float m_gain = 1.0;

    public:

        Sample(std::string path);
        ~Sample();

        bool eof = false;

        bool active = false;

        int loadSample();

        virtual void playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng);
        
        virtual void playSample(short int * transportAudioBuff, unsigned int writePointer, unsigned int bufferLeng, int note);
};