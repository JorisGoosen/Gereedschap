//This file simply opens an OpenGL window where a quad will be rendered and some simple shaders to display this fact, press escape to exit.
#include "../RenderSchermPerspectief.h"
#include "../geometrie/icosahedron.h"

int main()
{
	RenderSchermPerspectief scherm("Perspectief Demo");
	scherm.maakShader("perspectiefDemo", "shaders/perspectiefDemo.vert", "shaders/perspectiefDemo.frag");

	glClearColor(0,0,0,0);

	icosahedron ico;

	float rot = 0.0f;
	while(!scherm.stopGewenst())
	{
		scherm.setModelView(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)), rot, glm::vec3(0.0f, 1.0f, 0.0f)));
		scherm.bereidRenderVoor();
		ico.tekenJezelf();
		//scherm.renderQuad();
		scherm.rondRenderAf();


		rot += 0.01f;
	}
}