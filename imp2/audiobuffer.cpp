
#include "audiobuffer.hpp"

#include <iostream>

// static ID counter
int AudioBuffer::privateId = 0;

AudioBuffer::AudioBuffer()
{
  ID = privateId++;
  std::cout << "AudioBuffer::AudioBuffer() " << ID << std::endl;
}

int AudioBuffer::getID()
{
  return ID;
}

const std::vector<float>& AudioBuffer::get()
{
  //std::cout << "AudioBuffer::getPointer()  ID: " << ID << "  size = " << buffer.size() << std::endl;
  return buffer;
}

void AudioBuffer::nonRtSetSample(std::vector<float>& sample)
{
  buffer.swap(sample);
}

AudioBuffer::~AudioBuffer()
{
  std::cout << "AudioBuffer::~AudioBuffer() " << ID << std::endl;
}

