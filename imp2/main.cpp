
#include <iostream>

using namespace std;


#include "top.hpp"


Top* Top::instance = 0;

int main()
{
  
  Shared::get();
  
  cout << "Initializing Top" << endl;
  Top::init();
  
  // this function "blocks", it pauses the thread that enters it, and
  // gtkmm's callbacks of UI events will take over. It will sleep
  // if it doesn't have any work to do. It will return when the last
  // window that is show gets closed. Since we want to stop the program
  // when this happens, we just return 0 after this function call.
  cout << "Running GUI" << endl;
  Top::getGUI().run();
  
  return 0;
}

