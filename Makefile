CXXFLAGS 	= -fPIC -O3 -W -Wall -Wextra -std=c++2a `pkg-config --cflags glfw3 gl glew`
LIBFLAGS 	= `pkg-config --libs glfw3 glew gl`
OBJECTS 	= $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
DEMOS		= $(patsubst %.cpp,%,$(wildcard demos/*.cpp))
GEOMS	 	= $(patsubst %.cpp,%.o,$(wildcard geometrie/*.cpp))

all: gereedschap $(DEMOS) 

gereedschap: $(OBJECTS) $(GEOMS)
	g++ -shared $(CXXFLAGS) -o bin/libgereedschap.so $(OBJECTS) $(GEOMS) $(LIBFLAGS)
	ar rcs libgereedschap.a $(OBJECTS) $(GEOMS)

$(DEMOS): gereedschap $(wildcard demos/*.cpp)
	g++ $(CXXFLAGS) -o $(patsubst demos/%,bin/%,$@) $@.cpp $(OBJECTS) $(GEOMS) $(LIBFLAGS) -L. -lgereedschap

%.o: %.cpp geometrie/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) $(GEOMS) bin/*