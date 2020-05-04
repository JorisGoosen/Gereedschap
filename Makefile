CXXFLAGS = -g -W -Wall -Wextra -std=c++2a `pkg-config --cflags glfw3 gl glew`
#-O3
LIBFLAGS = `pkg-config --libs glfw3 glew gl`
OBJECTS := $(patsubst %.cpp,%.o,$(wildcard *.cpp))

all: $(OBJECTS) demo

demo: $(OBJECTS)
	g++ $(CXXFLAGS) -o demo $(OBJECTS) $(LIBFLAGS)

%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm *.o demo