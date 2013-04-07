
#ifndef SHOWCASE_DSP
#define SHOWCASE_DSP

#include <iostream>
#include <jack/jack.h>

#include "audiobuffer.hpp"

#include <list>

using namespace std;

class DSP
{
  public:
    DSP();
    ~DSP();
    
    // starts processing
    void run();
    
    // sets a new buffer for playing
    void setBuffer(AudioBuffer* newBuf);
    
    // disconnects from JACK
    void shutdown();
    
  private:
    unsigned long long playbackIndex;
    AudioBuffer* buffer;
    
    jack_client_t* client;
    
    jack_port_t* out_left;
    jack_port_t* out_right;
    
    int process (jack_nframes_t);
    static int static_process  (jack_nframes_t, void *);
};


#endif

