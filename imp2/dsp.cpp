
#include "dsp.hpp"

#include "top.hpp"

extern Top top;

DSP::DSP()
{
  cout << "DSP()" << endl;
  
  // open the client
  client = jack_client_open ( "showcase-2", JackNullOption , 0 , 0 );
  
  // register two output ports
  out_left  = jack_port_register( client,
                                  "out_left",
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsOutput,
                                  0 );
  
  out_right = jack_port_register( client,
                                  "out_right",
                                  JACK_DEFAULT_AUDIO_TYPE,
                                  JackPortIsOutput,
                                  0 );
  
  // register the static DSP callback
  jack_set_process_callback( client,
                             static_process,
                             static_cast<void*>(this) );
  
  // sample counter for playback
  playbackIndex = 0;
}

void DSP::run()
{
  // activate the client: ready to do processing
  jack_activate( client );
}

int DSP::static_process(jack_nframes_t nframes, void *instance)
{
  // remove static, by static_cast(instance) and calling member function
  return static_cast<DSP*>(instance)->process(nframes);
}

void DSP::addBuffer( boost::shared_ptr<AudioBuffer> buf )
{
  // copy the boost::shared_ptr to the local boost::shared_ptr
  // here the reference count of the old shared_ptr will decrease by 1
  buffer = buf;
}

int DSP::process(jack_nframes_t nframes)
{
  // process events from ringbuffer
  while ( top.getGuiToDsp()->can_read() )
  {
    top.getGuiToDsp()->read() ();
  }
  
  if ( buffer == 0 )
  {
    // no buffer loaded yet
    return 0;
  }
  
  float* outLBuffer = (float*) jack_port_get_buffer( out_left, nframes );
  float* outRBuffer = (float*) jack_port_get_buffer( out_right,nframes );
  
  // playback the currently loaded sample
  const std::vector<float>& buf = buffer->get();
  size_t size = buf.size();
  
  for ( int i = 0; i < nframes; i++)
  {
    if ( playbackIndex < size - 1 )
    {
      *outLBuffer++ = buf[playbackIndex];
      *outRBuffer++ = buf[playbackIndex];
      playbackIndex++;
    }
    else
    {
      playbackIndex = 0;
    }
  }
  
  // update GUI of current playback position
  float position = float(playbackIndex) / buf.size();
  
  if( top.getDspToGui()->can_write() )
  {
    // this is NON-RT! boost:function allocates memory!
    top.getDspToGui()->write( boost::bind( &GUI::playbackPosition, &top.getGUI(), position ) );
  }
  
  return 0;
}

void DSP::shutdown()
{
  // we disconnect from JACK: end of real-time code
  jack_deactivate( client );
  
  jack_port_unregister( client, out_left  );
  jack_port_unregister( client, out_right );
  
  jack_client_close( client );
  
  cout << "DSP::shutdown()" << endl;
}


DSP::~DSP()
{
  cout << "~DSP()" << endl;

}
