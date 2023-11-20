CXXFLAGS 	= -fPIC -O3 -W -Wall -Wextra -Werror=return-type -std=c++2a `pkg-config --cflags glfw3 glew libpng glm gl`
#CXXFLAGS 	= -fPIC -g3 -W -Wall -Wextra -Werror=return-type -std=c++2a `pkg-config --cflags glfw3 gl glew libpng`
LIBFLAGS 	= `pkg-config --libs glfw3 glew libpng glm gl`
OBJECTS 	= $(patsubst %.cpp,%.o,$(wildcard *.cpp)) 
DEMOS		= $(patsubst %.cpp,%,$(wildcard demos/*.cpp))
GEOMS	 	= $(patsubst %.cpp,%.o,$(wildcard geometrie/*.cpp))
SUBJS	 	= $(patsubst %.cpp,%.o,$(wildcard subjecten/*.cpp))

all: gereedschap $(DEMOS) 

gereedschap: $(OBJECTS) $(GEOMS) $(SUBJS)
	mkdir -p bin
	g++ -shared $(CXXFLAGS) -o bin/libgereedschap.so $(OBJECTS) $(GEOMS) $(SUBJS) $(LIBFLAGS)
	ar rcs libgereedschap.a $(OBJECTS) $(GEOMS) $(SUBJS)

$(DEMOS): gereedschap $(wildcard demos/*.cpp)
	g++ $(CXXFLAGS) -o $(patsubst demos/%,bin/%,$@) $@.cpp $(OBJECTS) $(GEOMS) $(SUBJS) $(LIBFLAGS) -L. -lgereedschap

%.o: %.cpp geometrie/%.cpp subjecten/%.cpp
	g++ $(CXXFLAGS) -c -o $@ $<

clean:
	rm $(OBJECTS) $(GEOMS) $(SUBJS) bin/* *.a