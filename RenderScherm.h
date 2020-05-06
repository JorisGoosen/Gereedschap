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

	void		bereidRenderVoor(const std::string & shader = "");
	void		rondRenderAf();

	bool		stopGewenst() { return glfwWindowShouldClose(_glfwScherm); }

	static 	void toetsVerwerker(GLFWwindow * scherm, 	int key, int scancode, int action, int mods);
	virtual void keyHandler(						int key, int scancode, int action, int mods);

	GLuint 		maakGeometrieShader(	const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand, const std::string &  geomshaderbestand);
	GLuint 		maakShader(				const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand);
	GLuint 		maakBerekenShader(		const std::string & shaderNaam,		const std::string &  shaderbestand);
	
	GLuint		geefProgrammaHandvat(const std::string & naam) const;
	GLuint		geefEnigeProgrammaHandvat() const;

	///Kan gebruikt worden door subklasses om nog extra dingen voor te bereiden
	virtual void extraVoorbereidingen(GLuint programma) {}
	
protected:
	float					_aspectRatio = 16.0f / 9.0f;
	std::string				_naam;

private:
	GLuint	slaShaderOp(const std::string & naam, GLuint shaderProgramma);

	GLFWwindow 			*	_glfwScherm = nullptr;

	std::map<std::string, GLuint>					_shaderProgrammas;
	static std::map<GLFWwindow *, RenderScherm*>	_schermen;
};