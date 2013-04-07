
#include "gui.hpp"

#include "top.hpp"
#include "dsp.hpp"

#include "event.hpp"

#include <sndfile.hh>

extern Top* top;

GUI::GUI() :
  kit (0 , 0)
{
  cout << "GUI() : Loading glade file" << endl;
  
  testIteration = 0;
  
  // load Glade file
  Glib::RefPtr<Gtk::Builder> refBuilder;
  
  try {
    std::string path = Glib::build_filename( Glib::get_current_dir() , "showcase.glade");
    cout << "Loading glade file from " << path << endl;
    refBuilder = Gtk::Builder::create_from_file( path );
  }
  catch (Gtk::BuilderError e) {
    cout << "Gtk::BuilderError loading mainwindow glade file: " << e.what() << endl;
    exit(1);
  }
  catch (Glib::FileError e) {
    cout << "Gtk::FileError loading mainwindow glade file: " << e.what() << endl;
    exit(1);
  }
  
  // extract widgets
  refBuilder->get_widget("window", window);
  refBuilder->get_widget("textView", textView);
  refBuilder->get_widget("waveviewBox", waveviewBox);
  refBuilder->get_widget("fileScrolledWindow", fileScrolledWindow);
  
  refBuilder->get_widget("fileQuit", fileQuit);
  fileQuit->signal_activate().connect ( sigc::mem_fun( *this, &GUI::quit ) );
  
  // esc key to close window
  window->signal_key_release_event().connect ( sigc::mem_fun( this, &GUI::on_key_release ) );
  
  fileSelector = new FileSelector();
  fileScrolledWindow->add( *fileSelector );
  
  waveview = new GWaveView();
  waveviewBox->add( *waveview );
  
  // get the buffer from the existing textView, and store it in a class var
  textBuffer = textView->get_buffer();
  
  // connect a callback that occurs approx every X ms to readRingbuffer()
  // if DSP sends a function to be called, at most it will take 100 ms
  // to be processed. Since they're not critical events for the sound output
  // the 100ms delay is negligable.
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &GUI::readRingbuffer), 100);
  
#ifdef SELF_TEST
  // make the program "self-test". runTests() will be called repeatedly, and 
  // the results of this will be showin in the profiling data
  cout << "GUI scheduling self tests" << endl;
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &GUI::runTests), 2000);
#endif // SELF_TEST
}

void GUI::playbackPosition( float pos )
{
  stringstream s;
  s << pos;
  
  printTextView( true, "new playback position:  " + s.str() );
}

bool GUI::readRingbuffer()
{
  //updateRingbufferStats();
  
  // call on top to do all the processing of events
  ::top->processGui();
  
  return true;
}

bool GUI::updateRingbufferStats()
{
  // calculate the remaining space in the ringbuffers and show in GUI
  size_t avaliableDtG =((top->getRingbufferSize() - top->getDspToGuiReadAvailable())*100) / top->getRingbufferSize();
  size_t avaliableGtD = (top->getGuiToDspWriteAvailable()*100) / top->getRingbufferSize();
  
  cout << "dsp->gui read available = " << avaliableDtG << "\t"
       << "gui->dsp write available= " << avaliableGtD << endl;
  
  return true;
}

GWaveView* GUI::getWaveview()
{
  return waveview;
}

bool GUI::on_key_release(GdkEventKey *event)
{
  if ( event->keyval == GDK_Escape )
  {
    quit();
  }
  
  return true;
}

void GUI::quit()
{
  cout << "GUI::quit()" << endl;
  kit.quit();
}

bool GUI::runTests()
{
  cout << "Gui::runTests() iteration: " << testIteration << endl;
  
  if ( testIteration < 10 )
  {
    if ( testIteration % 2 == 0 )
      loadSample( "440hz.wav" );
    else
      loadSample( "220hz.wav" );
    
    testIteration++;
  }
  else
  {
    // wait, let the memory settle, then quit
    Glib::signal_timeout().connect( sigc::bind_return(sigc::mem_fun(*this, &GUI::quit), 10000), true);
    return false;
  }
  
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &GUI::runTests), 4000);
  
  return false;
}

void GUI::loadSample(const std::string& str)
{
  string filePath = Glib::build_filename( Glib::get_current_dir(), "samples", str );
  
  SndfileHandle infile( filePath, SFM_READ );
  
  if ( infile.frames() == 0 )
  {
    cout << "GUI::loadSample() Infile.frames == 0" << endl;
    return;
  }
  
  std::vector<float> sampleBuffer( infile.frames(), 0.0 );
  
  //top->getGUI().printTextView( true, "Created vector buffer, reading data in...");
  
  infile.read( &sampleBuffer.at(0) , infile.frames() );
  
  
  // Create a new AudioBuffer, note its NOT memory-managed
  AudioBuffer* audioBuffer = new AudioBuffer();
  cout << "creating DSP audioBuffer  Addr: " << audioBuffer << endl;
  stringstream s;
  s.str("");
  s.clear();
  s << "GUI: Creating Audiobuffer instance " << audioBuffer->getID() << "  Addr: " << audioBuffer;
  top->getGUI().printTextView( true, s.str() );
  
  
  // *copy* the whole buffer for the GUI, we send an AudioBuffer* there too
  std::vector<float> cloneBuffer(sampleBuffer);
  AudioBuffer* guiAudioBuffer = new AudioBuffer();
  guiAudioBuffer->nonRtSetSample( cloneBuffer );
  cout << "creating GUI audioBuffer  Addr: " << guiAudioBuffer << endl;
  
  top->getGUI().printTextView( true, "GUI: Sending AudioBuffer to waveview widget" );
  top->getGUI().getWaveview()->setPlaybackBuffer( guiAudioBuffer );
  
  // swaps the contents of the vectors, audiobuffer will now have the
  // loaded sample data
  top->getGUI().printTextView( true, "GUI: Sending EventBufferTransfer to DSP" );
  audioBuffer->nonRtSetSample( sampleBuffer );
  
  // FIXME: Replace shared system with raw data pointers, owned by context
  EventBufferTransfer e = EventBufferTransfer( audioBuffer );
  top->guiToDspWrite( &e );
  
}

void GUI::printTextView( bool isGui, std::string str )
{
  if ( textBuffer )
  {
    textBuffer->insert( textBuffer->end(), str + "\n");
    
    // scroll to bottom
    Gtk::Adjustment* adj = textView->get_vadjustment();
    adj->set_value(adj->get_upper()); 
  }
  else
  {
    cerr << __FILE__ << " " << __LINE__ << " Error: textBuffer = 0 " << endl;
  }
}

void GUI::run()
{
  window->set_title("RT Showcase 1");
  window->set_default_size(800, 350);
  window->show_all();
  
  // start JACK processing
  top->getDSP().run();
  
  // run the GUI: blocking call until window is closed (program quit)
  kit.run( *window );
}

GUI::~GUI()
{
  cout << "~GUI()" << endl;

}
