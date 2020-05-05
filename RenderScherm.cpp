#include "RenderScherm.h"
#include <iostream>

using namespace glm;

std::map<GLFWwindow *, RenderScherm*>	RenderScherm::_schermen;

RenderScherm::RenderScherm(std::string Naam, size_t W, size_t H) 
: _aspectRatio(float(W) / float(H)), _naam(Naam)
{
	std::cout << "RenderScherm " << _naam << " created!" << std::endl;

    if (_schermen.size() == 0 && !glfwInit())
		throw std::runtime_error("Failed to intialize glfw");
        
    _glfwScherm = glfwCreateWindow(W, H, _naam.c_str(), nullptr, nullptr);

    if (!_glfwScherm)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window!");
    }

	

    glfwMakeContextCurrent(_glfwScherm);

	if(_schermen.size() == 0)
	{
		GLenum err = glewInit();
		if (GLEW_OK != err)
		{
			std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
			throw std::runtime_error("Failed to initalize glew...");
		}
		std::cout << "Status: Using GLEW " << glewGetString(	GLEW_VERSION) << std::endl;
	}
	
	_schermen[_glfwScherm] = this;

	glfwSwapInterval(1);

	glfwSetKeyCallback(_glfwScherm, keyHandler);
}

void RenderScherm::keyHandler(GLFWwindow * scherm, int key, int scancode, int action, int mods)
{
	if(_schermen.count(scherm) > 0)
		_schermen[scherm]->keyHandler(key, scancode, action, mods);
}

void RenderScherm::keyHandler(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(_glfwScherm, 1);
}

RenderScherm::~RenderScherm()
{
	_schermen.erase(_glfwScherm);

   	glfwDestroyWindow(_glfwScherm);

	if(_schermen.size() == 0)
		glfwTerminate();
}

void RenderScherm::prepareForRender()
{
	glfwMakeContextCurrent(_glfwScherm);

	int width, height;
	
	glfwGetFramebufferSize(_glfwScherm, &width, &height);
	_aspectRatio = width / (float) height;

	//std::cout << "Windowsize: " << width << "x" << height << std::endl;

	glViewport(0, 0, width, height);
}

void RenderScherm::finishRender()
{
	glFlush();
	glfwSwapBuffers(_glfwScherm);
	glfwPollEvents();
}