
#ifndef SHOWCASE_FILESELECTOR
#define SHOWCASE_FILESELECTOR

#include <vector>
#include <iostream>

#include <gtkmm/drawingarea.h>

// This widget derives from Gtk::DrawingArea, allowing us to draw custom
// shapes and surfaces. The file selector will be placed in the main
// window, that functionality is provided by DrawingArea too.
class FileSelector : public Gtk::DrawingArea
{
  public:
    FileSelector();
    
    bool redraw();
  
  private:
    int widgetSizeX;
    int widgetSizeY;
    
    int block;
    
    std::string currentDir;
    std::list<std::string> fileList;
    
    // helper functions
    void setWidgetSizeFromList();
    
    //Override default signal handler:
    bool on_expose_event			(GdkEventExpose* event);
    bool on_button_press_event(GdkEventButton* event);
    
    // drop functions
    void dropFunction( const Glib::RefPtr<Gdk::DragContext>& context, int x, int y,
                       const Gtk::SelectionData& selection_data, guint info, guint time);
    
    void dragFunction( const Glib::RefPtr<Gdk::DragContext>& context,
                       Gtk::SelectionData& selection_data, guint info, guint time);
    
    // For Right-Click Actions
    void on_menu_file_popup_edit();
    void on_menu_file_popup_generic();
};

#endif

