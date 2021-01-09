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
#include <algorithm>

#include "wrgvOnderOpslag.h"	

	
const long double 	PI  = 3.141592653589793238L;
const float 		FPI = 3.141592653589793238f;



class weergaveScherm
{
public:
	typedef std::function<void(int key, int scancode, int action, int mods)> keyHandlerFunc;

				weergaveScherm(std::string Naam = "weergaveScherm", size_t W = 1280, size_t H = 720, size_t multiSamples = 1);
				~weergaveScherm();

	void		bereidRenderVoor(const std::string & verwerker = "", bool wisScherm = true);
	void		rondRenderAf();

	bool		stopGewenst() { return glfwWindowShouldClose(_glfwScherm); }

	static 	void toetsVerwerker(GLFWwindow * scherm, 	int key, int scancode, int action, int mods);
	virtual void keyHandler(							int key, int scancode, int action, int mods);
	static	void setCustomKeyhandler(keyHandlerFunc customHandler) { _customHandler = customHandler; }

	GLuint 		maakVlakVerdelingsShader(	const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand, const std::string &  vlakEvaluatieBestand, const std::string &  vlakControleBestand = "");
	GLuint 		maakGeometrieShader(		const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand, const std::string &  geomshaderbestand);
	GLuint 		maakShader(					const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand);
	GLuint 		maakRekenShader(			const std::string & shaderNaam,		const std::string &  shaderbestand);
	
	GLuint		geefProgrammaHandvat(const std::string & naam) 	const;
	GLuint		geefEnigeProgrammaHandvat() 					const;
	GLuint		huidigProgramma()								const { return _huidigProgramma; }

	glm::vec2 	laadTextuurUitPng(const std::string & bestandsNaam, const std::string & textuurNaam,  bool herhaalS = true, bool herhaalT = true, unsigned char ** imgData = nullptr);
	void 		bindTextuur(const std::string & textuurNaam, GLuint bindPlek) const;

	void		doeRekenVerwerker(const std::string & verwerker, glm::uvec3 groepGroottes, std::function<void()> renderVoorbereiding);

	///Kan gebruikt worden door subklasses om nog extra dingen voor te bereiden
	virtual void extraVoorbereidingen(GLuint /*programma*/) { }
	
protected:
	float					_aspectRatio = 16.0f / 9.0f;
	std::string				_naam;
	GLuint					_huidigProgramma;

private:
	GLuint	slaShaderOp(const std::string & naam, GLuint shaderProgramma);

	GLFWwindow 			*	_glfwScherm = nullptr;

	std::map<std::string, GLuint>					_shaderProgrammas,
													_texturen;
	static std::map<GLFWwindow *, weergaveScherm*>	_schermen;

	static keyHandlerFunc _customHandler;
};