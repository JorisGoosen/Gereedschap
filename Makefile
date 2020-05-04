all: textfile.o RenderScherm.o RenderSubBuffer.o

%.o: %.cpp
	g++ -O3 -W -Wall -Wextra -std=c++2a -c -o $@ $< -lGL -lGLU -lGLEW -lGLFW

clean:
	rm *.o