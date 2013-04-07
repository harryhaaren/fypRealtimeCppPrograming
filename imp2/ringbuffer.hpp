
#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <jack/ringbuffer.h>
#include <memory>

// boost headers for inter-thread comms
#include <boost/function.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/bind.hpp>

// this file is adapted from Florian Paul Schmidt's "JASS" project,
// which in turn is based on the Jack ringbuffer code. See here for the source:
// https://github.com/fps/jass/blob/master/ringbuffer.h

// - Author:
// Florian Paul Schmidt (mista.tapas@gmx.net)

// - Updated to get read / write space available:
// Harry van Haaren (harryhaaren@gmail.com)

// - License:
// Gnu General Public License version 3.0 or later



/**
  T needs to be a default constructable type.. And it has to have valid
  copy constructor/assignment operator

  Note that this class needs to construct n objects of type T (with n == size)  so 
  that the places in the ringbuffer become assignable

  Note that the objects remain in the ringbuffer until they are overwritten again
  when the ringbuffer returns to the current position the next time around. I.e. a
  read() does not assign a T() to the read object as that could cause destructors
  to be called, etc..

  Note that read() creates a copy of the T, so the T(const T&) should be non blocking
*/

template <class T> 
class TemplateRingBuffer {
  unsigned int size;

  jack_ringbuffer_t *jack_ringbuffer;

  public:
    TemplateRingBuffer(unsigned int size) : size(size) {
      jack_ringbuffer = jack_ringbuffer_create(sizeof(T) * size);
      
      for (unsigned int i = 0; i < size; ++i) {
        T *t = new (jack_ringbuffer->buf + sizeof(T) * i) T();
        t = t;
      }
    }

    ~TemplateRingBuffer() {
      for (unsigned int i = 0; i < size; ++i) {
        ((T*)(jack_ringbuffer->buf + sizeof(T) * i))->~T();
      }

      jack_ringbuffer_free(jack_ringbuffer);
    }

    bool can_write() {
      if (jack_ringbuffer_write_space(jack_ringbuffer) >= sizeof(T)) {
        return true;
      }

      return false;
    }
    
    int getReadAvailable() {
      return jack_ringbuffer_read_space(jack_ringbuffer);
    }
    
    int getWriteAvailable() {
      return jack_ringbuffer_write_space(jack_ringbuffer);
    }

    void write(const T &t) {
      jack_ringbuffer_data_t rb_data[2];
      jack_ringbuffer_get_write_vector(jack_ringbuffer, rb_data);
      *((T*)rb_data->buf) = t;
      jack_ringbuffer_write_advance(jack_ringbuffer, sizeof(T));
    }

    bool can_read() {
      if (jack_ringbuffer_read_space(jack_ringbuffer) >= sizeof(T)) {
        return true;
      }

      return false;
    }

    T& read() {
      jack_ringbuffer_data_t rb_data[2];
      jack_ringbuffer_get_read_vector(jack_ringbuffer, rb_data);
      jack_ringbuffer_read_advance(jack_ringbuffer, sizeof(T));
      return *((T*)rb_data->buf);
    }
};

#endif
