
#ifndef SHOWCASE_TOP
#define SHOWCASE_TOP

#include <iostream>

// for jack_ringbuffer_
#include <jack/ringbuffer.h>

class DSP;
class GUI;
class EventBase;

using namespace std;

class Top
{
  public:
    Top();
    ~Top();
    
    // for calling functions on these objects in the same thread
    DSP& getDSP();
    GUI& getGUI();
    
    // process the ringbuffer events
    void processDsp();
    void processGui();
    
    // write the passed event into the ringbuffer
    int guiToDspWrite( EventBase* );
    int dspToGuiWrite( EventBase* );
    
    size_t getDspToGuiReadAvailable();
    size_t getGuiToDspWriteAvailable();
    
    size_t getRingbufferSize();
  
  private:
    // memory, gets allocated sizeof(EventBase), to peek at contents of ringbuffer
    EventBase* guiToDspMem;
    EventBase* dspToGuiMem;
    
    // GUI and DSP instance pointers
    DSP* dsp;
    GUI* gui;
    
    size_t ringbufferSize;
    
    // ringbuffer pointers for communication between threads
    jack_ringbuffer_t* guiToDsp;
    jack_ringbuffer_t* dspToGui;
    
    Top(Top const&);
    void operator=(Top const&);
};

#endif

