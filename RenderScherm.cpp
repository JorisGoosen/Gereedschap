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

	glfwSetKeyCallback(_glfwScherm, toetsVerwerker);
}

void RenderScherm::toetsVerwerker(GLFWwindow * scherm, int key, int scancode, int action, int mods)
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

void RenderScherm::bereidRenderVoor(const std::string & shader)
{
	glfwMakeContextCurrent(_glfwScherm);

	int width, height;
	
	glfwGetFramebufferSize(_glfwScherm, &width, &height);
	_aspectRatio = width / (float) height;

	//std::cout << "Windowsize: " << width << "x" << height << std::endl;

	glViewport(0, 0, width, height);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	if(shader == "" && _shaderProgrammas.size() == 1)				glUseProgram(_shaderProgrammas.begin()->second);
	else if(shader != "" && _shaderProgrammas.count(shader) > 0)	glUseProgram(_shaderProgrammas[shader]);
	else															std::cerr << "Kon niet bepalen welke shader gebruikt moest worden, dus nu maar geen..." << std::endl;
		
	extraVoorbereidingen();
}

void RenderScherm::rondRenderAf()
{
	glFlush();
	glfwSwapBuffers(_glfwScherm);
	glfwPollEvents();
}

GLuint RenderScherm::slaShaderOp(const std::string & naam, GLuint shaderProgramma)
{
	_shaderProgrammas[naam] = shaderProgramma;

	return shaderProgramma;
}

GLuint RenderScherm::maakGeometrieShader(	const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand, const std::string &  geomshaderbestand)
{
	return slaShaderOp(shaderNaam, creategeomshader(vertshaderbestand, fragshaderbestand, geomshaderbestand));
}

GLuint RenderScherm::maakShader(				const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand)
{
	return slaShaderOp(shaderNaam, createshader(vertshaderbestand, fragshaderbestand));
}

GLuint RenderScherm::maakBerekenShader(		const std::string & shaderNaam,		const std::string &  shaderbestand)
{
	return slaShaderOp(shaderNaam, createcomputeshader(shaderbestand));
}