#include <pulse/pulseaudio.h>
#include <string.h>
#include <iostream>

#include "Player.h"

u_int sampleRate = 44100;
u_char channels = 1;
u_char sampleSize = sizeof(short int);
u_int bufferLength_msec = 100;

// int inputDevice = 0;
int outputDevice = 0;

int outputDeviceIndex = 0;
std::string outputDeviceID;

pa_threaded_mainloop * mloop = pa_threaded_mainloop_new();

//##PA calbacks##
//context state change
void on_state_change(pa_context *c, void *userdata){
    pa_threaded_mainloop_signal(mloop, 0);
}
//gather output devices
void on_dev_sink(pa_context * c, const pa_sink_info *info, int eol, void * udata){

    if (eol != 0){
        pa_threaded_mainloop_signal(mloop, 0);
        return; 
    }

    std::cout << "sink: " << outputDeviceIndex << ": " << info->name << '\n';

    if (outputDeviceIndex == outputDevice){

        outputDeviceID = info->name;

        outputDeviceIndex++;

        pa_threaded_mainloop_signal(mloop, 0);
        return; 
    }

    outputDeviceIndex++;
}
//callback for when new data is available in the stream
void on_io_complete(pa_stream *s, size_t nbytes, void *udata){

    pa_threaded_mainloop_signal(mloop, 0);
}
void on_drain_complete(pa_stream * s, int success, void *udata){
    //drain gang

    //this is really stupid but i think having the cout as an extra delay here 
    //might be fixing a weird race condition or somthing preventing an error??
    std::cout << "drain status: " << success << '\n';

    pa_threaded_mainloop_signal(mloop, 0);
}

//do and unreference pa operation
void do_op(pa_operation * op){

    int r;

    for(;;){
        r = pa_operation_get_state(op);

        if (r == PA_OPERATION_DONE || r == PA_OPERATION_CANCELLED) break;

        pa_threaded_mainloop_wait(mloop);
    }

    pa_operation_unref(op);
}

//get device IDs using device numbers
void getDeviceIDs(pa_context * ctx){
    //output 
    do_op(pa_context_get_sink_info_list(ctx, on_dev_sink, NULL));

    std::cout << "out: "<< outputDeviceID << '\n';
}

void playback(pa_context * ctx){

    //create and set up stream
    pa_sample_spec spec;
    spec.format = PA_SAMPLE_S16LE;
    spec.rate = sampleRate;
    spec.channels = channels;
    pa_stream *stm = pa_stream_new(ctx, "sequencer", &spec, NULL);

    pa_buffer_attr attr;
    memset(&attr, 0xff, sizeof(attr));
    attr.tlength = spec.rate * sampleSize * spec.channels * bufferLength_msec / 1000; //set buffer max length

    pa_stream_set_write_callback(stm, on_io_complete, NULL); //set callback func for new data available
    pa_stream_connect_playback(stm, outputDeviceID.data(), &attr, PA_STREAM_NOFLAGS, NULL, NULL); //connect to device using device ID

    //wait untill stream is ready
    for (;;) {
        int r = pa_stream_get_state(stm);
        if (r == PA_STREAM_READY) break;
        else if (r == PA_STREAM_FAILED) std::cout << "stream error\n";

        pa_threaded_mainloop_wait(mloop);
    }

    //init sample creator
    Player player(sampleRate, 1, 200);

    //if the player's samples have loaded correctly
    if (player.samplesState == 0){
        //audio processing loop
        std::cout << "starting playback\n";

        bool eof = false;
        int cnt = 0;

        while(!player.eof){

            size_t n = pa_stream_writable_size(stm);

            if (n == 0) {
                pa_threaded_mainloop_wait(mloop);
                continue;
            }

            void * buffer;
            pa_stream_begin_write(stm, &buffer, &n);

            //get sample values from player 
            player.populateSamples((short int *)buffer, n / 2);

            //process

            pa_stream_write(stm, buffer, n, NULL, 0, PA_SEEK_RELATIVE);       
        }
    } else {
        std::cout << player.samplesState << " samples incorrectly loaded\n";
    }

    do_op(pa_stream_drain(stm, on_drain_complete, NULL));

    pa_stream_disconnect(stm);
    pa_stream_unref(stm);
}

void parseArgs(int argc, char **argv){

    if (argc > 1){

        if (strcmp(argv[1], "-d") == 0){
            outputDevice = strtol(argv[2], nullptr, 0);

            if (outputDevice < 0 || outputDevice > 20) outputDevice = 0;
        }
    }
}

int main(int argc, char **argv){

    std::cout << "starting\n";

    parseArgs(argc, argv);

    // create/start loop and lock
    pa_threaded_mainloop_start(mloop);
    pa_threaded_mainloop_lock(mloop);

    pa_mainloop_api * mlapi = pa_threaded_mainloop_get_api(mloop); //get api
    pa_context * ctx = pa_context_new_with_proplist(mlapi, "sequencer", NULL); //get context with application name and property list
    pa_context_set_state_callback(ctx, on_state_change, NULL); // set context state change call back function

    /* Connect the context */
    if (pa_context_connect(ctx, NULL, PA_CONTEXT_NOFLAGS, NULL) < 0) {
        fprintf(stderr, "pa_context_connect() failed.\n");
        return -1;
    }

    //wait untill context is ready
    while (pa_context_get_state(ctx) != PA_CONTEXT_READY){
        pa_threaded_mainloop_wait(mloop);
    }

    //set the device ID strings using the device number
    getDeviceIDs(ctx);
    //start audio
    playback(ctx);

    //closing
    pa_context_disconnect(ctx);
    pa_context_unref(ctx);

    pa_threaded_mainloop_unlock(mloop);
    pa_threaded_mainloop_stop(mloop);
    pa_threaded_mainloop_free(mloop);

    return 0;
}
