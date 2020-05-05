#pragma once
#include <GL/glew.h>
#include <string>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <map>

#include "RenderSubBuffer.h"	

	
const long double 	PI  = 3.141592653589793238L;
const float 		FPI = 3.141592653589793238f;

class RenderScherm
{
public:
		RenderScherm(std::string Naam = "RenderScherm", size_t W = 1280, size_t H = 720);
		~RenderScherm();

	void		prepareForRender();
	void		finishRender();

	bool		closeRequested() { return glfwWindowShouldClose(_glfwScherm); }

	static 	void keyHandler(GLFWwindow * scherm, int key, int scancode, int action, int mods);
	virtual void keyHandler(int key, int scancode, int action, int mods);

	
protected:
	float					_aspectRatio = 16.0f / 9.0f;
	std::string				_naam;

private:
	GLFWwindow 			*	_glfwScherm = nullptr;
	
	static std::map<GLFWwindow *, RenderScherm*>	_schermen;
};