CXXFLAGS 	= -fPIC -g -W -Wall -Wextra -Werror=return-type -std=c++2a `pkg-config --cflags glfw3 gl glew libpng`
LIBFLAGS 	= `pkg-config --libs glfw3 glew gl libpng`
OBJECTS 	= $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
DEMOS		= $(patsubst %.cpp,%,$(wildcard demos/*.cpp))
GEOMS	 	= $(patsubst %.cpp,%.o,$(wildcard geometrie/*.cpp))

all: gereedschap $(DEMOS) 

gereedschap: $(OBJECTS) $(GEOMS)
	mkdir -p bin
	g++ -shared $(CXXFLAGS) -o bin/libgereedschap.so $(OBJECTS) $(GEOMS) $(LIBFLAGS)
	ar rcs libgereedschap.a $(OBJECTS) $(GEOMS)

$(DEMOS): gereedschap $(wildcard demos/*.cpp)
	g++ $(CXXFLAGS) -o $(patsubst demos/%,bin/%,$@) $@.cpp $(OBJECTS) $(GEOMS) $(LIBFLAGS) -L. -lgereedschap

%.o: %.cpp geometrie/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) $(GEOMS) bin/* *.a