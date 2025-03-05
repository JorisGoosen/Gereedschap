//This file simply opens an OpenGL window where a quad will be rendered and some simple shaders to display this fact, press escape to exit.
#include "../weergaveSchermPerspectief.h"
#include "../geometrie/geodesisch.h"
#include <iostream>

int main()
{
	weergaveSchermPerspectief scherm("Geodesie");

	scherm.maakShader("geoDemo", "shaders/geoDemo.vert", "shaders/geoDemo.frag");

	glClearColor(0,0,0,0);

	geodesisch geo(6);

	float rot = 0.0f;
	while(!scherm.stopGewenst())
	{
		scherm.herberekenProjectie();
		scherm.zetModelZicht(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.5f)), rot, glm::vec3(0.0f, 1.0f, 0.0f)));
		scherm.bereidRenderVoor();

		geo.tekenJezelf();
		glErrorToConsole("Woppaloppa Mainloop ");
		scherm.rondRenderAf();
		glErrorToConsole("rondRenderAf: ");

		rot += 0.0007f;
	}
}