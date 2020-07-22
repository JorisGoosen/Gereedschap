#include "RenderSchermPerspectief.h"
#include <iostream>

using namespace glm;


RenderSchermPerspectief::RenderSchermPerspectief(std::string Naam, size_t W, size_t H, size_t samples) 
: RenderScherm(Naam, W, H, samples)
{
	RecalculateProjection();
}

/*void RenderSchermPerspectief::keyHandler(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(_glfwScherm, 1);
}*/

RenderSchermPerspectief::~RenderSchermPerspectief()
{
}

void RenderSchermPerspectief::extraVoorbereidingen(GLuint programma)
{
	glEnable(GL_DEPTH_TEST);

	RecalculateProjection();	

	glUniformMatrix4fv(glGetUniformLocation(programma, "projectie"), 1, GL_FALSE, glm::value_ptr(_projection));
	glUniformMatrix4fv(glGetUniformLocation(programma, "modelView"), 1, GL_FALSE, glm::value_ptr(_modelView));

	glErrorToConsole("RenderSchermPerspectief::extraVoorbereidingen()");
}
