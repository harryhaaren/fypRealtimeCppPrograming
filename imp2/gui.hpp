
#ifndef SHOWCASE_GUI
#define SHOWCASE_GUI

#include <iostream>

#include <gtkmm.h>

#include "waveview.hpp"
#include "fileselector.hpp"

using namespace std;

class GUI
{
  public:
    GUI();
    ~GUI();
    
    // launches the User interface
    void run();
    
    // automatically load samples for profiling / testing purposes
    bool runTests();
    
    // update stats on ringbuffer usage
    bool updateRingbufferStats();
    
    // loads a sample
    void loadSample(std::string);
    
    // reads events from the ringbuffer, and exectues them
    bool readRingbuffer();
    
    // passes a pointer to the waveform to another class
    GWaveView* getWaveview();
    
    // prints a message in the textbox
    void printTextView( std::string );
    
    // updates the playhead in the GUI
    void playbackPosition( float pos );
    
    // quits the GUI loop. This is called from DSP shutdown()
    void quit();
  
  private:
    Gtk::Main kit;
    
    Gtk::Window* window;
    Gtk::MenuItem* fileQuit;
    
    GWaveView*   waveview;
    Gtk::Box*    waveviewBox;
    
    Gtk::TextView* textView;
    Glib::RefPtr<Gtk::TextBuffer> textBuffer;
    
    Gtk::ScrolledWindow* fileScrolledWindow;
    FileSelector* fileSelector;
    
    int testIteration;
    
    bool on_key_release(GdkEventKey *event);
};


#endif

