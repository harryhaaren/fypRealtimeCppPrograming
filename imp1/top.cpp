
#include "top.hpp"

#include "dsp.hpp"
#include "gui.hpp"

#include <iostream>
#include <sstream>

// for mlockall()
#include <sys/mman.h>

#include "event.hpp"

using namespace std;

GUI& Top::getGUI()
{
  return *gui;
}

DSP& Top::getDSP()
{
  return *dsp;
}

Top::Top( )
{
  cout << "Top()" << endl;
  
  ringbufferSize = sizeof( EventBase )*500;
  
  guiToDsp = jack_ringbuffer_create( ringbufferSize );
  dspToGui = jack_ringbuffer_create( ringbufferSize );
  
  if ( !guiToDsp || !dspToGui )
  {
    exit(0);
  }
  
  // for peeking at the contents of an event before reading it
  guiToDspMem = static_cast<EventBase*>( malloc( sizeof( EventBase ) ) );
  dspToGuiMem = static_cast<EventBase*>( malloc( sizeof( EventBase ) ) );
  
  jack_ringbuffer_mlock( guiToDsp );
  jack_ringbuffer_mlock( dspToGui );
  
  // setup the DSP and GUI instances
  gui = new GUI();
  dsp = new DSP();
  
  mlockall( MCL_CURRENT | MCL_FUTURE ); 
  
}

Top::~Top()
{
  delete(dsp);
  //delete(gui);
  
  jack_ringbuffer_free( guiToDsp );
  jack_ringbuffer_free( dspToGui );
}

void Top::processDsp()
{
  // check if there's anything to read
  int availableRead = jack_ringbuffer_read_space( guiToDsp );
  
  while ( availableRead >= sizeof(EventBase) )
  {
    jack_ringbuffer_peek( guiToDsp, (char*)guiToDspMem, sizeof(EventBase) );
    
    EventBase* e = static_cast<EventBase*>( guiToDspMem );
    
    // have to check the size against the event size (as opposed to the EventBase
    // size check earlier. Just because the write thread has written the EventBase
    // doesn't mean the whole event is written (2 memcpy writes when buffer wraps).
    // this check gaurantees that the whole event is available, and can be run
    if ( availableRead >= e->size() )
    {
      switch ( e->type() )
      {
        case EventBase::EVENT_BUFFER_TRANSFER:
        {
          if ( availableRead >= sizeof(EventBufferTransfer) )
          {
            EventBufferTransfer ev( (AudioBuffer*)0);
            jack_ringbuffer_read( guiToDsp, (char*)&ev, sizeof(EventBufferTransfer) );
            
            // pass the pointer to DSP
            dsp->setBuffer( ev.audioBufferPtr );
          }
          break;
        }
      }
    }
    // update available read, and loop over events
    availableRead = jack_ringbuffer_read_space(guiToDsp);
  }
}

// GUI thread calls this function periodically to update itself
void Top::processGui()
{
  // check if there's anything to read
  int availableRead = jack_ringbuffer_read_space(dspToGui);
  
  while ( availableRead >= sizeof(EventBase) )
  {
    jack_ringbuffer_peek( dspToGui, (char*)dspToGuiMem, sizeof(EventBase) );
    
    EventBase* e = static_cast<EventBase*>( dspToGuiMem );
    
    // again, have to check in case buffer is wrapping, and thread gets paused
    // inbetween the two memcpy calls, leaving a possible non-full event
    if ( availableRead >= e->size() )
    {
      switch ( e->type() )
      {
        case EventBase::EVENT_BUFFER_TRANSFER:
        {
          // create an empty of this event type
          EventBufferTransfer ev( (AudioBuffer*)0 );
          
          // read the event from the buffer
          jack_ringbuffer_read( dspToGui, (char*)&ev, sizeof(ev) );
          
          stringstream s;
          s << "GUI: Deleting AudioBuffer instance " << ev.audioBufferPtr->getID() << "  Addr: "
            << ev.audioBufferPtr;
          gui->printTextView( true, s.str() );
          
          AudioBuffer* tmp = ev.audioBufferPtr;
          cout << "Top deleting DSP audioBuffer  Addr: " << tmp << endl;
          
          
          // do the action: de-allocate buffer
          delete tmp;
          
          break;
        }
        case EventBase::EVENT_GUI_PRINT:
        {
          // create an empty of this event type
          EventGuiPrint ev;
          
          // read the event from the buffer
          jack_ringbuffer_read( dspToGui, (char*)&ev, ev.size() );
          
          // do the action: print it
          gui->printTextView( false, ev.getMessage() );
          
          break;
        }
        case EventBase::EVENT_PLAYBACK_POSITION:
        {
          // create an empty of this event type
          EventPlaybackPosition ev;
          
          // read the event from the buffer
          jack_ringbuffer_read( dspToGui, (char*)&ev, sizeof(ev) );
          
          // do the action: set the playback position
          gui->getWaveview()->setPlaybackPosition( ev.position );
          
          break;
        }
        default:
        {
          cout << "Top::processGUI() Recieved unknown event type " << e->type() << endl;
        }
      } // switch
    } // if ( available >= e->size() )
    else
    {
      // return if the EventBase was written but the whole event was not. This 
      // breaks out of the while loop, and the EventBase which is still in the
      // ringbuffer will be read on the next call of this function
      return;
    }
    // update available read, and see if there's more events 
    availableRead = jack_ringbuffer_read_space( dspToGui );
  }
}

size_t Top::getDspToGuiReadAvailable()
{
  return jack_ringbuffer_read_space( dspToGui );
}

size_t Top::getGuiToDspWriteAvailable()
{
  return jack_ringbuffer_write_space( guiToDsp );
}

size_t Top::getRingbufferSize()
{
  return ringbufferSize;
}

int Top::guiToDspWrite( EventBase* e )
{
  size_t size = e->size();
  
  size_t writeSize = jack_ringbuffer_write_space(guiToDsp);
  
  if ( writeSize >= size )
  {
    jack_ringbuffer_write( guiToDsp, (const char*)e, size );
  }
  else
  {
    cerr << "Top: Error! no space in gui->dsp ringbuffer" << endl;
  }
  return 0;
}


int Top::dspToGuiWrite( EventBase* e )
{
  size_t size = e->size();
  
  size_t writeSize = jack_ringbuffer_write_space(dspToGui);
  
  if ( writeSize >= size )
  {
    jack_ringbuffer_write( dspToGui, (const char*)e, e->size() );
  }
  else
  {
    // error: the ringbuffer is full. Using cerr causes non-RT behaviour, but
    // the programmer sees the error during testing. In production code this
    // would be removed.
    cerr << "Top: Error! no space in dsp->gui ringbuffer" << endl;
  }
  
  return 0;
}
