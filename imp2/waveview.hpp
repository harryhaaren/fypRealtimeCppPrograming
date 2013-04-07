
#ifndef GWAVEVIEW
#define GWAVEVIEW

#include <vector>
#include <gtkmm.h>

#include <boost/shared_ptr.hpp>

class AudioBuffer;

class GWaveView : public Gtk::DrawingArea
{
  public:
    GWaveView();
    
    void setPlaybackPosition(float);
    void setPlaybackBuffer( boost::shared_ptr<AudioBuffer> );

  protected:
    int width;
    int height;
    
    bool draw();
    
    float playbackPosition;
    
    // keep a shared_ptr to the buffer
    boost::shared_ptr<AudioBuffer> audioBuffer;
    
    // this is the GTK "draw" function, it's where we'll draw lines onto the widget
    bool on_expose_event(GdkEventExpose* event);
    
    // keep a Cairo::Context local, draw the waveform on it, then just blit
    // that Context to the window. Much faster redrawing
    bool newWaveform;
    Cairo::RefPtr<Cairo::ImageSurface> waveformSurface;
    Cairo::RefPtr<Cairo::Context>      waveformContext;
};

#endif // GWAVEVIEW

