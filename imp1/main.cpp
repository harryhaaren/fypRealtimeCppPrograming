
#include <iostream>
#include <vector>

#include <unistd.h>

#include <sndfile.hh>

using namespace std;


#include "top.hpp"
#include "event.hpp"
#include "audiobuffer.hpp"
#include "gui.hpp"


Top* top = 0;

int main()
{
  cout << "Initializing Top" << endl;
  top = new Top();
  
  // this function "blocks", it pauses the thread that enters it, and
  // gtkmm's callbacks of UI events will take over. It will sleep
  // if it doesn't have any work to do. It will return when the last
  // window that is show gets closed. Since we want to stop the program
  // when this happens, we just return 0 after this function call.
  cout << "Running GUI" << endl;
  
  top->getGUI().run();
  
  return 0;
}

