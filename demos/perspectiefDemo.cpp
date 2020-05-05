//This file simply opens an OpenGL window where a quad will be rendered and some simple shaders to display this fact, press escape to exit.
#include "../RenderSchermPerspectief.h"

int main()
{
	RenderSchermPerspectief scherm("Perspectief Demo");
	scherm.maakShader("perspectiefDemo", "shaders/perspectiefDemo.vert", "shaders/perspectiefDemo.frag");

	glClearColor(1,0,0,0);
	
	while(!scherm.stopGewenst())
	{
		scherm.bereidRenderVoor();
		//scherm.renderQuad();
		scherm.rondRenderAf();
	}
}