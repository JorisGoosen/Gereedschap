#include "RenderScherm.h"
#include "textfile.h"

int main()
{
	RenderScherm scherm;


	GLuint barelyFunctionalShader =	createshader("demo.vert", "demo.frag");

	glClearColor(0, 0, 0, 1);
	

	while(!scherm.closeRequested())
	{
		scherm.prepareForRender();
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glUseProgram(barelyFunctionalShader);
		
		scherm.RenderQuad();
		scherm.finishRender();
	}
}