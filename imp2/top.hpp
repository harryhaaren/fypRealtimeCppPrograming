
#ifndef SHOWCASE_TOP
#define SHOWCASE_TOP

#include <iostream>

#include "dsp.hpp"
#include "gui.hpp"

#include "shared.hpp"

#include "ringbuffer.hpp"

using namespace std;

class Top
{
  public:
    static void init();
    
    static DSP& getDSP();
    static GUI& getGUI();
    
    static TemplateRingBuffer<boost::function<void()> >* getGuiToDsp();
    static TemplateRingBuffer<boost::function<void()> >* getDspToGui();
    
    static size_t getRingbufferSize(){return ringbufferSize;}
    static size_t getDspToGuiReadAvailable(){return dspToGui->getReadAvailable();}
    static size_t getGuiToDspWriteAvailable(){return guiToDsp->getWriteAvailable();}
  
  private:
    // "self" instance pointer, static
    static Top* instance;
    
    // GUI and DSP instance pointers
    static DSP* dsp;
    static GUI* gui;
    
    static int ringbufferSize;
    
    // ringbuffer pointers for communication between threads
    static TemplateRingBuffer<boost::function<void()> >* guiToDsp;
    static TemplateRingBuffer<boost::function<void()> >* dspToGui;
    
    Top ( );
    
    Top(Top const&);
    void operator=(Top const&);
};

#endif

