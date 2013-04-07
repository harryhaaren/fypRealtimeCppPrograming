
#ifndef EVENT
#define EVENT

#include <iostream>

#include <stdint.h>
#include <cstring>

// for EventBufferTransfer*
class AudioBuffer;

using namespace std;

class EventBase
{
  public:
    enum {
        EVENT_BUFFER_TRANSFER = 0,
        EVENT_GUI_PRINT,
        EVENT_PLAYBACK_POSITION,
        EVENT_ACK,
    };
    
    virtual ~EventBase() {}
    
    virtual int type() = 0;
    virtual uint32_t size() = 0;
};

// Transfers an AudioBuffer*
class EventBufferTransfer : public EventBase
{
  public:
    int type() { return int(EVENT_BUFFER_TRANSFER); }
    uint32_t size() { return sizeof(EventBufferTransfer); }
    
    AudioBuffer* audioBufferPtr;
    
    EventBufferTransfer(){};
    EventBufferTransfer(AudioBuffer* buf)
    {
      audioBufferPtr = buf;
    }
};


// Sets the playback position of the file
class EventPlaybackPosition : public EventBase
{
  public:
    int type() { return int(EVENT_PLAYBACK_POSITION); }
    uint32_t size() { return sizeof(EventPlaybackPosition); }
    
    float position;
    
    EventPlaybackPosition(){position = 0.f;}
    EventPlaybackPosition(float p)
    {
      position = p;
    }
    float getPosition()
    {
      return position;
    };
};


// prints the string S in the GUI console
class EventGuiPrint : public EventBase
{
  public:
    int type() { return int(EVENT_GUI_PRINT); }
    uint32_t size() { return sizeof(EventGuiPrint); }
    
    char stringArray[50];
    
    EventGuiPrint(){}
    EventGuiPrint(const char* s)
    {
      if ( strlen( s ) > 50 )
      {
        // this will be called from an RT context, and should be removed from
        // production code. It is here for the programmer to notice when they
        // are using code which causes too long a message.
        cout << "EventGuiPrint() error! Size of string too long!" << endl;
      }
      else
      {
        // move the sting into this event
        strcpy( &stringArray[0], s );
      }
    }
    char* getMessage()
    {
      return &stringArray[0];
    }
};

#endif
