#include "weergaveSchermPerspectief.h"
#include <iostream>

using namespace glm;


weergaveSchermPerspectief::weergaveSchermPerspectief(std::string Naam, size_t W, size_t H, size_t samples) 
: weergaveScherm(Naam, W, H, samples)
{
	RecalculateProjection();
}

/*void weergaveSchermPerspectief::keyHandler(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(_glfwScherm, 1);
}*/

weergaveSchermPerspectief::~weergaveSchermPerspectief()
{
}

void weergaveSchermPerspectief::extraVoorbereidingen(GLuint programma)
{
	glEnable(GL_DEPTH_TEST);

	RecalculateProjection();	

	glUniformMatrix4fv(glGetUniformLocation(programma, "projectie"), 1, GL_FALSE, glm::value_ptr(_projection));
	glErrorToConsole("weergaveSchermPerspectief::extraVoorbereidingen() -> projectie");

	glUniformMatrix4fv(glGetUniformLocation(programma, "modelView"), 1, GL_FALSE, glm::value_ptr(_modelView));
	glErrorToConsole("weergaveSchermPerspectief::extraVoorbereidingen() -> modelView");
}
