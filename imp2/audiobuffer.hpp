
#ifndef SHOWCASE_AUDIOBUFFER
#define SHOWCASE_AUDIOBUFFER

#include <vector>
#include <boost/shared_ptr.hpp>

// AudioBuffer stores float samples in a big vector. The vector can be
// accessed only by const reference, so its state is immutable. This
// allows the GUI to read the data safely from the DSP state.

class AudioBuffer
{
  public:
    AudioBuffer();
    ~AudioBuffer();
    
    int getID();
    
    const std::vector<float>& get();
    
    void nonRtSetSample(std::vector<float>& sample);
  
  protected:
    static int privateId;
    int ID;
    
    std::vector<float> buffer;
};

typedef boost::shared_ptr<AudioBuffer> AudioBufferPtr;

#endif

