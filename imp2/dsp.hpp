
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
    
    // activate JACK
    void run();
    
    void addBuffer( boost::shared_ptr<AudioBuffer> buf );
    
    void shutdown();
    
  private:
    
    unsigned long playbackIndex;
    boost::shared_ptr<AudioBuffer> buffer;
    
    jack_client_t* client;
    
    jack_port_t* out_left;
    jack_port_t* out_right;
    
    int process (jack_nframes_t);
    static int static_process  (jack_nframes_t, void *);
};


#endif

