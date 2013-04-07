
#include "gui.hpp"
#include "top.hpp"


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
  
  fileSelector = new FileSelector();
  fileScrolledWindow->add( *fileSelector );
  
  // esc key to close window
  window->signal_key_release_event().connect ( sigc::mem_fun( this, &GUI::on_key_release ) );
  
  waveview = new GWaveView();
  waveviewBox->add( *waveview );
  
  // get the buffer from the existing textView, and store it in a class var
  textBuffer = textView->get_buffer();
  
  printTextView( "" );
  
  // connect a callback that occurs approx every X ms to readRingbuffer()
  // if DSP sends a function to be called, at most it will take 100 ms
  // to be processed. Since they're not critical events for the sound output
  // the 100ms delay is negligable.
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &GUI::readRingbuffer), 100);
  
  // this callback connects to Shared::cleanup(). Everytime a buffer's use count
  // drops to 1, Shared will remove it from memory. 
  Glib::signal_timeout().connect( sigc::mem_fun( Shared::get(), &Shared::cleanup), 50);

#ifdef SELF_TEST
  // make the program "self-test". runTests() will be called repeatedly, and 
  // the results of this will be showin in the profiling data
  cout << "GUI scheduling self tests" << endl;
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &GUI::runTests), 2000);
#endif // SELF_TEST
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
    Glib::signal_timeout().connect( sigc::bind_return( sigc::mem_fun(*this, &GUI::quit), 10000), true);
    return false;
  }
  
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &GUI::runTests), 4000);
  
  return false;
}


bool GUI::updateRingbufferStats()
{
  // calculate the remaining space in the ringbuffers and show in GUI
  cout << "top size " << top->getRingbufferSize();
  
  size_t avaliableDtG = top->getDspToGuiReadAvailable() / top->getRingbufferSize();
  size_t avaliableGtD = top->getGuiToDspWriteAvailable();
  
  cout << "dsp->gui read available = " << avaliableDtG << "\t"
       << "gui->dsp write available= " << avaliableGtD << endl;
  
  return true;
}


bool GUI::on_key_release(GdkEventKey *event)
{
  if ( event->keyval == GDK_Escape )
  {
    quit();
  }
  
  return true;
}

void GUI::playbackPosition( float pos )
{
  waveview->setPlaybackPosition( pos );
}

bool GUI::readRingbuffer()
{
  // uncomment to check the ringbuffer space available
  //updateRingbufferStats();
  
  // process events from ringbuffer
  while ( top->getDspToGui()->can_read() )
  {
    //cout << "Reading event in GUI thread" << endl;
    top->getDspToGui()->read() ();
  }
  
  return true;
}

GWaveView* GUI::getWaveview()
{
  return waveview;
}

void GUI::quit()
{
  cout << "GUI::quit()" << endl;
  kit.quit();
}

void GUI::loadSample(std::string str)
{
  string filePath = Glib::build_filename( Glib::get_current_dir(), "samples", str );
  
  SndfileHandle infile( filePath, SFM_READ );
  
  if ( infile.frames() == 0 )
  {
    cout << "Fileselector::on_button_press_event() Infile.frames == 0" << endl;
    return;
  }
  
  std::vector<float> sampleBuffer( infile.frames(), 0.0 );
  
  top->getGUI().printTextView("Created vector buffer, reading data in...");
  
  infile.read( &sampleBuffer.at(0) , infile.frames() );
  
  top->getGUI().printTextView( "Creating Audiobuffer instance..." );
  
  // Create a new AudioBuffer
  boost::shared_ptr<AudioBuffer> tmp = boost::shared_ptr<AudioBuffer>( new AudioBuffer() );
  
  top->getGUI().printTextView( "Swapping buffers..." );
  
  // swaps the contents of the vectors, audiobuffer will now have the
  // loaded sample data
  tmp->nonRtSetSample( sampleBuffer );
  
  cout << "Adding instance to Shared..." << endl;
  Shared::get()->addAudio( tmp );
  
  // update the waveform
  top->getGUI().getWaveview()->setPlaybackBuffer( tmp );
  
  // send the Buffer to DSP:
  if( top->getGuiToDsp()->can_write() )
  {
    top->getGuiToDsp()->write( boost::bind( &DSP::addBuffer, &top->getDSP(), tmp ) );
  }
  
  // Manually destroy the local instance of the shared pointer. This is so that
  // the use count isn't one higher than it should be. Logically the local
  // scoped shared_ptr<> should -1 from the use_count when it goes out of scope,
  // but it doesn't unless  we manually destroy it.
  // That is why we have the manual destructor call here.
  tmp.~shared_ptr<AudioBuffer>();
}

void GUI::printTextView( std::string str )
{
  textBuffer->insert( textBuffer->end(), str + "\n" );
  
  Gtk::Adjustment* adj = textView->get_vadjustment();
  adj->set_value(adj->get_upper()); 
  
}

void GUI::run()
{
  window->set_title("RT Showcase 2");
  window->set_default_size(800, 350);
  window->show_all();
  
  // start JACK processing
  top->getDSP().run();
  
  kit.run( *window );
}

GUI::~GUI()
{
  cout << "~GUI()" << endl;

}
