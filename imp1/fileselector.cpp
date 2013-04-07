
#include "fileselector.hpp"

#include <glibmm/fileutils.h>

#include <sstream>
#include <boost/shared_ptr.hpp>
#include "audiobuffer.hpp"

#include "top.hpp"

#include "dsp.hpp"
#include "gui.hpp"

extern Top* top;


using namespace std;
using namespace boost;

FileSelector::FileSelector()
{
  //std::cout << "Enterin FileSelector contructor" << std::endl;
  add_events(Gdk::EXPOSURE_MASK | Gdk::BUTTON_PRESS_MASK);
  signal_button_press_event().connect(sigc::mem_fun(*this, &FileSelector::on_button_press_event) );
  
  // drag target types
  std::list<Gtk::TargetEntry> listTargets;
  listTargets.push_back( Gtk::TargetEntry("AUDIO_FILE_STRING") );
  
  // drag source
  drag_source_set(listTargets);
  
  // drag function connect
  signal_drag_data_get().connect(sigc::mem_fun(*this, &FileSelector::dragFunction ));
  
  widgetSizeX = 150;
  widgetSizeY = 18;
  
  
  currentDir = Glib::build_filename( Glib::get_current_dir() , "samples");
  
  // ensure the samples folder exists
  if ( not Glib::file_test( currentDir, Glib::FILE_TEST_IS_DIR ) )
  {
    cout << "Error: Samples folder does not exist. Quitting" << endl;
    exit(1);
  }
  
  Glib::Dir dir ( currentDir );
  
  // empty current contents
  fileList.clear();
  
  // loop forever, until read_next() returns ""
  while( true )
  {
    // retrive the next filename from Glib::Dir
    string filename = dir.read_name();
    
    if ( filename.compare("") == 0 )
    {
      // there are no more filenames in this directory
      break;
    }
    
    unsigned int found = filename.find(".wav");
    
    if (found != string::npos )
    {
      //cout << "found wav @ " << found << ", contents =  " << *i << endl;
      // add the filename to the .wav fileList
      fileList.push_back( filename );
    }
  }
  
  // finally resize widget based on new list size
  setWidgetSizeFromList();
}

bool FileSelector::redraw()
{
  // force our program to redraw the entire widget.
  Glib::RefPtr<Gdk::Window> win = get_window();
  if (win)
  {
      Gdk::Rectangle r(0, 0, widgetSizeX, widgetSizeY);
      win->invalidate_rect(r, false);
  }
  return true;
}

void FileSelector::setWidgetSizeFromList()
{
  widgetSizeY = 18 * fileList.size();
  //cout << "FileSelector::setWidgetSizeFromList() setting widget size to " << widgetSizeY << endl;
  set_size_request( widgetSizeX, widgetSizeY );
}


bool FileSelector::on_expose_event(GdkEventExpose* event)
{
  // This is where we draw on the window
  Glib::RefPtr<Gdk::Window> window = get_window();
  
  if(window)    // Only run if Window does exist
  {
    // clip to the area indicated by the expose event so that we only redraw
    // the portion of the window that needs to be redrawn
    Cairo::RefPtr<Cairo::Context> cr = window->create_cairo_context();
    cr->rectangle(event->area.x, event->area.y,
            event->area.width, event->area.height);
    cr->clip();
    
    cr->rectangle(event->area.x, event->area.y,
        event->area.width, event->area.height);
    cr->set_source_rgb( 0.2, 0.2, 0.2 );
    cr->fill();
    
    std::list<std::string>::iterator i = fileList.begin();
    
    int x = 0;
    int y = 0;
    
    
    for(; i != fileList.end(); i++ )
    {
      // background colour
      cr->set_source_rgb( 0.05, 0.05, 0.05 );
      cr->set_line_width(0.9);
      cr->rectangle (x,y, widgetSizeX, 17);
      cr->fill();
      
      // draw "play square" on left of block
      cr->set_source_rgb( 0.2, 0.2, 0.2 );
      cr->rectangle (x+1, y+1, 15, 15);
      cr->fill();
      
      cr->select_font_face ("Impact" , Cairo::FONT_SLANT_NORMAL, Cairo::FONT_WEIGHT_NORMAL);
      cr->set_font_size ( 13 );
      cr->move_to ( x + 22, (y + 13) );
      cr->set_source_rgb( 0.5, 0.5, 0.5 );
      cr->show_text ( *i );
      
      
      y += 18;
    }
    
  }
  return true;
}

void FileSelector::on_menu_file_popup_edit()
{
   std::cout << "The Edit menu item was selected." << std::endl;
}

void FileSelector::on_menu_file_popup_generic()
{
   std::cout << "A popup menu item was selected." << std::endl;
}

bool FileSelector::on_button_press_event(GdkEventButton* event)
{
  block = ((int)event->y) / 18;
  //cout << "Button click on block " << block << endl;
  
  if ( block < fileList.size() )
  {
    std::list<std::string>::iterator i = fileList.begin();
    std::advance(i, block);
    std::string str = *i;
    
    // sample loading code in GUI class
    ::top->getGUI().loadSample( str );
  }
  
  return true;
}

void FileSelector::dragFunction( const Glib::RefPtr<Gdk::DragContext>& context,
                       Gtk::SelectionData& selection_data, guint info, guint time)
{
  cout << "FileSelector::dragFunction() called!" << endl;
  
  // drag target is of string type, send
  cout << "DRAG from block " << block << endl;
  
  list<string>::iterator i = fileList.begin();
  
  for( int counter = 0; i != fileList.end(); i++ )
  {
    if ( counter == block )
    {
      // send data to drop
      std::string fullFilename = Glib::build_filename( currentDir, *i );
      selection_data.set( selection_data.get_target(), 8 /* 8 bits format */, (guchar*) fullFilename.c_str(), fullFilename.size() );
    }
    counter++;
  }
  
}







