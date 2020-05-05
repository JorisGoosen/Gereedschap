//This file simply opens an OpenGL window where a quad will be rendered and some simple shaders to display this fact, press escape to exit.
#include "../RenderSchermQuad.h"

int main()
{
	RenderSchermQuad scherm("Quad Demo");
	scherm.maakShader("quadDemo", "shaders/quadDemo.vert", "shaders/quadDemo.frag");

	while(!scherm.stopGewenst())
	{
		scherm.bereidRenderVoor();
		scherm.renderQuad();
		scherm.rondRenderAf();
	}
}