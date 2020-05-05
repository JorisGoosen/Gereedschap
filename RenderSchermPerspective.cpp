#include "RenderSchermPerspective.h"
#include <iostream>

using namespace glm;


RenderSchermPerspective::RenderSchermPerspective(std::string Naam, size_t W, size_t H) 
: RenderScherm(Naam, W, H)
{
	RecalculateProjection();
}

/*void RenderSchermPerspective::keyHandler(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(_glfwScherm, 1);
}*/

RenderSchermPerspective::~RenderSchermPerspective()
{
}

