
#include "shared.hpp"

Shared* Shared::instance = 0;


// for getting to GUI
#include "top.hpp"

// for printTextView() function
#include "gui.hpp"

extern Top top;


bool Shared::cleanup()
{
  //top.getGUI().printTextView( "Shared::cleanup()" );
  
  for (std::list<boost::shared_ptr<AudioBuffer> >::iterator it = audioList.begin(); it != audioList.end(); )
  {
    if ( it->unique() )
    {
      cout << "Shared::clean() Removed AudioBuffer!" << endl;
      it = audioList.erase(it);
    }
    else
    {
      //cout << "Shared::clean() Buffer has " << it->use_count() <<" uses!" << endl;
      ++it;
    }
  }
  
  // stay connected to signal handler
  return true;
}

const AudioBuffer* Shared::getBuffer(int ID)
{
  for (std::list<boost::shared_ptr<AudioBuffer> >::iterator it = audioList.begin(); it != audioList.end(); )
  {
    if ( (*it)->getID() == ID )
    {
      cout << "Shared::clean() Retrieved AudioBuffer id " << ID << " !" << endl;
      return it->get();
    }
    else
    {
      ++it;
    }
  }
  return 0;
}

Shared* Shared::get()
{
  if (instance) return instance;
  
  cout << "creating new Shared" << endl;
  return (instance = new Shared());
}

void Shared::addAudio(boost::shared_ptr<AudioBuffer> d)
{
  audioList.push_front(d);
}
