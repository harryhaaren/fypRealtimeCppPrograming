
#include "waveview.hpp"
#include "audiobuffer.hpp"

#include <cmath>

#include "top.hpp"

#include "gui.hpp"

extern Top* top;

GWaveView::GWaveView()
{
  newWaveform = false;
  
  // Gives "Exposure" events to the widget, we need the for when we want
  // to redraw the widget!
  add_events(Gdk::EXPOSURE_MASK);
  
  // set default widget size
  set_size_request( 400, 100 );
  
  // set the re-draw timeout, so the GUI redraws smoothly
  Glib::signal_timeout().connect( sigc::mem_fun(*this, &GWaveView::draw ), 1000/10);
}

void GWaveView::setPlaybackPosition(float pos)
{
  playbackPosition = pos;
  //cout << "playback pos:  " << pos << endl;
}

void GWaveView::setPlaybackBuffer( boost::shared_ptr<AudioBuffer> buf )
{
  // old buffer will get automatically de-allocated by Shared::cleanup() when
  // gui calls into it. When we assign a new value here, the ref count will drop
  // to 1. This means shared will clean it up
  
  //top->getGUI().printTextView( true, "GUI: Recieved new AudioBuffer in waveview widget" );
  audioBuffer = buf;
  
  newWaveform = true;
  
  draw();
}

bool GWaveView::draw()
{
  // force our program to redraw the entire widget.
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win)
  {
    Gdk::Rectangle r(0, 0, get_allocation().get_width(),get_allocation().get_height());
    win->invalidate_rect(r, false);
  }
  return true;
}

bool GWaveView::on_expose_event(GdkEventExpose* event)
{
    // This is where we draw on the window
    Glib::RefPtr<Gdk::Window> window = get_window();
    
    if( window )    // Only run if Window does exist
    {
      Gtk::Allocation allocation = get_allocation();
      width = allocation.get_width();
      height = allocation.get_height();
      
      // coordinates for the center of the window
      int xc, yc;
      xc = width / 2;
      yc = height / 2;
      
      Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
      
      // clip to the area indicated by the expose event so that we only redraw
      // the portion of the window that needs to be redrawn
      cr->rectangle(event->area.x, event->area.y,event->area.width, event->area.height);
      cr->clip();
      
      int x = 0;
      int y = 0;
      int xSize = width;
      int ySize = height;
      
      // works but a bit simple
      cr -> move_to( x        , y         );
      cr -> line_to( x + xSize, y         );
      cr -> line_to( x + xSize, y + ySize );
      cr -> line_to( x        , y + ySize );
      cr -> close_path();
      
      // Draw outline shape
      cr -> set_source_rgb (0.1,0.1,0.1);
      cr -> fill();
      
      if ( not audioBuffer )
      {
        // draw X
        cr -> move_to( x        , y         );
        cr -> line_to( x + xSize, y + ySize );
        cr -> move_to( x        , y + ySize );
        cr -> line_to( x + xSize, y         );
        cr -> set_source_rgb (0.2,0.2,0.2);
        cr -> stroke();
        
        // draw text
        std::string text = "No file loaded";
        cr->move_to( x + (xSize/2.f) - 65, y + (ySize/2.f) + 10 );
        cr -> set_source_rgb (0.6,0.6,0.6);
        cr->set_font_size( 20 );
        cr->show_text( text );
        
        return true;
      }
      
      if ( newWaveform )
      {
        if ( !waveformSurface )
        {
          // create the waveform surface and context
          waveformSurface = Cairo::ImageSurface::create ( Cairo::FORMAT_ARGB32, width, height);
          waveformContext = Cairo::Context::create ( waveformSurface ); 
        }
        
        // clear the surface
        waveformContext->rectangle(0, 0, width, height);
        waveformContext->set_source_rgb (0.1,0.1,0.1);
        waveformContext->fill();
        
        // don't draw every sample
        int sampleCountForDrawing = -1;
        
        float currentTop = 0.f;
        float previousTop = 0.f;
        float currentSample = 0.f;
        
        const std::vector<float>& sample = audioBuffer->get();
        
        // loop over each pixel value we need
        for( int p = 0; p < width; p++ )
        {
          // find how many samples per pixel
          int samplesPerPix = sample.size() / width;
          
          float average = 0.f;
          
          // calc value for this pixel
          for( int i = 0; i < samplesPerPix; i++ )
          {
            float tmp = sample.at(i + (p * samplesPerPix) );
            if ( tmp < 0 )
            {
              tmp = -tmp;
            }
            average += tmp;
          }
          average =(average / samplesPerPix);
          //average =  10 * log10( average / samplesPerPix ); // log representation
          
          // draw the average pixel value
          waveformContext->move_to( p, y + (ySize/2) - (average * ySize )  ); // top
          waveformContext->line_to( p, y + (ySize/2) + (average  * ySize )  ); // bottom
        }
        
        // stroke the waveform
        waveformContext->set_source_rgb (1.0,1.0,1.0);
        waveformContext->stroke();
        
        newWaveform = false;
      }
      
      // blit waveformContext onto the cr context
      //cr->set_antialias( Cairo::ANTIALIAS_NONE );
      cr->set_source ( waveformSurface, 0.0, 0.0);
      cr->rectangle (0.0, 0.0, waveformSurface->get_width(), waveformSurface->get_height());
      cr->clip();
      cr->paint();
      
      // draw the playhead
      cr->move_to( xSize * playbackPosition, 0     );
      cr->line_to( xSize * playbackPosition, ySize );
      cr -> set_source_rgba(1.0,0.0,0.0, 0.6);
      cr->stroke();
    }
  return true;
} // on_expose_event()

