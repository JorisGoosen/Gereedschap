CXXFLAGS 	= -O3 -W -Wall -Wextra -std=c++2a `pkg-config --cflags glfw3 gl glew`
LIBFLAGS 	= `pkg-config --libs glfw3 glew gl`
OBJECTS 	= $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
DEMOS		= $(patsubst %.cpp,%,$(wildcard demos/*.cpp))

all: $(OBJECTS) $(DEMOS)

$(DEMOS): $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $@.cpp $(OBJECTS) $(LIBFLAGS)

%.o: %.cpp 
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) $(DEMOS)