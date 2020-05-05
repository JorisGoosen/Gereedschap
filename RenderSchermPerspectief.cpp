#include "RenderSchermPerspectief.h"
#include <iostream>

using namespace glm;


RenderSchermPerspectief::RenderSchermPerspectief(std::string Naam, size_t W, size_t H) 
: RenderScherm(Naam, W, H)
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

