CXXFLAGS 	= -O3 -W -Wall -Wextra -std=c++2a `pkg-config --cflags glfw3 gl glew`
LIBFLAGS 	= `pkg-config --libs glfw3 glew gl`
OBJECTS 	= $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
DEMOS		= $(patsubst %.cpp,%,$(wildcard demos/*.cpp))
GEOMS	 	= $(patsubst %.cpp,%.o,$(wildcard geometrie/*.cpp))

all: $(OBJECTS) $(DEMOS)

$(DEMOS): $(OBJECTS) $(GEOMS) $(wildcard demos/*.cpp)
	g++ $(CXXFLAGS) -o $@ $@.cpp $(OBJECTS) $(GEOMS) $(LIBFLAGS)

%.o: %.cpp geometrie/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) $(DEMOS) $(GEOMS)