CXXFLAGS 	= -O3 -W -Wall -Wextra -std=c++2a `pkg-config --cflags glfw3 gl glew`
LIBFLAGS 	= `pkg-config --libs glfw3 glew gl`
OBJECTS 	= $(patsubst %.cpp,%.o,$(wildcard *.cpp))
DEMOS		= quadDemo

all: $(OBJECTS) $(DEMOS)

$(DEMOS): $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) $(LIBFLAGS)

%.o: %.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm *.o demo