
#include "top.hpp"


// for mlockall()
#include <sys/mman.h>

#include <iostream>

using namespace std;

DSP* Top::dsp = 0;
GUI* Top::gui = 0;

int Top::ringbufferSize = 500;

TemplateRingBuffer<boost::function<void()> >* Top::guiToDsp = new TemplateRingBuffer<boost::function<void()> >(ringbufferSize);
TemplateRingBuffer<boost::function<void()> >* Top::dspToGui = new TemplateRingBuffer<boost::function<void()> >(ringbufferSize);

void Top::init()
{
  if ( instance == 0 )
  {
    // instantiate itself
    cout << "New Top()" << endl;
    instance = new Top();
  }
}

DSP& Top::getDSP()
{
  return *dsp;
}

GUI& Top::getGUI()
{
  return *gui;
}

Top::Top( )
{
  // setup the DSP and GUI instances
  dsp = new DSP();
  gui = new GUI();
}



TemplateRingBuffer<boost::function<void()> >* Top::getGuiToDsp()
{
  return guiToDsp;
}

TemplateRingBuffer<boost::function<void()> >* Top::getDspToGui()
{
  return dspToGui;
}

