
# Makefile to compile the elements of the program piece by piece
# and then compile the whole program out of the pieces.

CC=g++

# Override to allow the dynamic adding of different flags
override CFLAGS+=-c -Wall `pkg-config gtkmm-2.4 --cflags`
override LDFLAGS+=-ljack -lsndfile `pkg-config gtkmm-2.4 --libs`

SOURCES=waveview.cpp dsp.cpp gui.cpp top.cpp audiobuffer.cpp shared.cpp fileselector.cpp main.cpp
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=showcase2


all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@

clean:
	rm *.o showcase2
