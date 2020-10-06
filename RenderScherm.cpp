#include "RenderScherm.h"
#include <iostream>

using namespace glm;

RenderScherm::keyHandlerFunc RenderScherm::_customHandler = nullptr;

std::map<GLFWwindow *, RenderScherm*>	RenderScherm::_schermen;

RenderScherm::RenderScherm(std::string Naam, size_t W, size_t H, size_t samples) 
: _aspectRatio(float(W) / float(H)), _naam(Naam)
{
	std::cout << "RenderScherm " << _naam << " created!" << std::endl;

    if (_schermen.size() == 0 && !glfwInit())
		throw std::runtime_error("Failed to intialize glfw");

	if(samples > 1)
		glfwWindowHint(GLFW_SAMPLES, samples);
        
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

	if(samples > 1)
		glEnable(GL_MULTISAMPLE); 
}

void RenderScherm::toetsVerwerker(GLFWwindow * scherm, int key, int scancode, int action, int mods)
{
	if(_customHandler)
		_customHandler(key, scancode, action, mods);

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

	GLuint programma = 0;

	if(shader == "" && _shaderProgrammas.size() == 1)				programma = _shaderProgrammas.begin()->second;
	else if(shader != "" && _shaderProgrammas.count(shader) > 0)	programma = _shaderProgrammas[shader];
	else															std::cerr << "Kon niet bepalen welke shader gebruikt moest worden, dus nu maar geen..." << std::endl;
		
	glUseProgram(programma);
	glErrorToConsole("glUseProgram(...): ");
	extraVoorbereidingen(programma);
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

	glErrorToConsole("slaShaderOp: ");

	return shaderProgramma;
}

GLuint RenderScherm::maakVlakVerdelingsShader(
	const std::string & shaderNaam,		
	const std::string & vertshaderbestand, 	
	const std::string & fragshaderbestand, 
	const std::string & vlakEvaluatieBestand, 
	const std::string & vlakControleBestand)
{
	return slaShaderOp(shaderNaam, createtesselationshader(vertshaderbestand, fragshaderbestand, vlakEvaluatieBestand, vlakControleBestand));
}

GLuint RenderScherm::maakGeometrieShader(
	const std::string & shaderNaam,		
	const std::string & vertshaderbestand, 	
	const std::string & fragshaderbestand, 
	const std::string & geomshaderbestand)
{
	return slaShaderOp(shaderNaam, creategeomshader(vertshaderbestand, fragshaderbestand, geomshaderbestand));
}

GLuint RenderScherm::maakShader(const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand)
{
	return slaShaderOp(shaderNaam, createshader(vertshaderbestand, fragshaderbestand));
}

GLuint RenderScherm::maakBerekenShader(const std::string & shaderNaam,		const std::string &  shaderbestand)
{
	return slaShaderOp(shaderNaam, createcomputeshader(shaderbestand));
}

GLuint RenderScherm::geefProgrammaHandvat(const std::string & naam) const
{
	if(_shaderProgrammas.count(naam) > 0)
		return _shaderProgrammas.at(naam);

	throw std::runtime_error("Er wordt gepoogd het handvat op te vragen van een shader programma genaamd \"" + naam + "\" maar dat bestaat niet voor zover het scherm weet...");
}


GLuint RenderScherm::geefEnigeProgrammaHandvat() const
{
	if(_shaderProgrammas.size() == 1)
		return _shaderProgrammas.begin()->second;

	throw std::runtime_error("Er wordt gepoogd het enige maar er zijn er '"+ std::to_string(_shaderProgrammas.size())+"'...");
}

void RenderScherm::laadTextuurUitPng(const std::string bestandsNaam, const std::string textuurNaam)
{
	size_t breedte, hoogte, kanalen;
	png_byte * data = laadPNG(bestandsNaam, breedte, hoogte, kanalen);

	if(!data) 
		throw std::runtime_error("Could not load '" + bestandsNaam + "'!");

	unsigned int texture;
	glGenTextures(1, &texture);  

	glBindTexture(GL_TEXTURE_2D, texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, breedte, hoogte, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

	glGenerateMipmap(GL_TEXTURE_2D);

	glErrorToConsole("RenderScherm::laadTextuurUitPng: ");

	_texturen[textuurNaam] = texture;

	delete data;
}