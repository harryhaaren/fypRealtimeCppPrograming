
#ifndef SHOWCASE_SHARED
#define SHOWCASE_SHARED

#include <list>
#include <iostream>
#include <boost/shared_ptr.hpp>

#include "audiobuffer.hpp"

using namespace std;

class Shared {
  public:
    std::list<boost::shared_ptr<AudioBuffer> > audioList;
    
    static Shared* get();

    void addAudio(boost::shared_ptr<AudioBuffer> d);
    const AudioBuffer* getBuffer(int ID);
    
    bool cleanup();

    ~Shared() { instance = 0; }

  protected:
    // singleton instance
    Shared(){}
    static Shared* instance;
};

#endif


