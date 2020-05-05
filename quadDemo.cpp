//This file simply opens an OpenGL window where a quad will be rendered and some simple shaders to display this fact, press escape to exit.

#include "RenderSchermQuad.h"
#include "textfile.h"

int main()
{
	RenderSchermQuad scherm("Quad Demo");

	GLuint barelyFunctionalShader =	createshader("shaders/quadDemo.vert", "shaders/quadDemo.frag");

	glClearColor(0, 0, 0, 1);
	

	while(!scherm.closeRequested())
	{
		scherm.prepareForRender();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(barelyFunctionalShader);
		
		scherm.renderQuad();
		scherm.finishRender();
	}
}