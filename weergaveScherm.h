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
#include <set>
#include <algorithm>
#include <functional>
#include "wrgvOnderOpslag.h"	

	
const long double 	PI  = 3.141592653589793238L;
const float 		FPI = 3.141592653589793238f;



class weergaveScherm
{
public:
	typedef std::function<void(int key, int scancode, int action, int mods)> toetsVerwerkerFunc;

				weergaveScherm(std::string Naam = "weergaveScherm", size_t W = 1280, size_t H = 720, size_t multiSamples = 1, bool volledigScherm = true);
				~weergaveScherm();

	void		bereidWeergevenVoor(const std::string & verwerker = "", bool wisScherm = true);
    void		_bereidWeergevenVoor(const std::string &shader, bool wisScherm, int breedte, int hoogte);
	void		laadOmgeving();
    void 		rondWeergevenAf();

    bool		stopGewenst() { return glfwWindowShouldClose(_glfwScherm); }

	static 	void toetsVerwerkerCentraal(GLFWwindow * scherm, 	int key, int scancode, int action, int mods);
	virtual void toetsVerwerker(								int key, int scancode, int action, int mods);
	static	void zetEigenToetsVerwerker(toetsVerwerkerFunc eigenVerwerker) { _eigenVerwerker = eigenVerwerker; }

	GLuint 		maakVlakVerdelingsShader(	const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand, const std::string &  vlakEvaluatieBestand, const std::string &  vlakControleBestand = "");
	GLuint 		maakGeometrieShader(		const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand, const std::string &  geomshaderbestand);
	GLuint 		maakShader(					const std::string & shaderNaam,		const std::string &  vertshaderbestand, 	const std::string &  fragshaderbestand);
	GLuint 		maakRekenShader(			const std::string & shaderNaam,		const std::string &  shaderbestand);
	
	GLuint		geefProgrammaHandvat(const std::string & naam) 	const;
	GLuint		geefEnigeProgrammaHandvat() 					const;
	GLuint		huidigProgramma()								const { return _huidigProgramma; }


	glm::ivec2 	laadTextuurUitPng(	const std::string & bestandsNaam, const std::string & textuurNaam,  bool herhaalS = true, bool herhaalT = true, bool mipmap = true, GLenum internalFormat=GL_RGBA, unsigned char ** imgData = nullptr);
	void 		bindTextuur(		const std::string & textuurNaam, GLuint actieveTextuur) const;
	void 		bindTextuurPlaatje(	const std::string & textuurNaam, GLuint actieveTextuur, bool schrijven = true, bool lezen = true) const;
	void		maakVolumeTextuur(	const std::string & textuurNaam, glm::uvec3 dimensies, unsigned char * data = nullptr);
	void		maakLijnTextuur(	const std::string & textuurNaam, size_t lengte, GLenum internalFormat =  GL_RGBA16 , void * data = nullptr, GLenum dataFormat = GL_RGBA, GLenum dataType = GL_FLOAT);
	GLuint		maakTextuur(		const std::string & textuurNaam, size_t breedte, size_t hoogte, bool herhaalS = false, bool herhaalT = false, bool mipmap = false, GLenum internalFormat = GL_RGBA, void * data = nullptr, 	GLenum dataFormat = GL_RGBA, 	GLenum dataType = GL_UNSIGNED_BYTE);
	void		laadData(			const std::string & textuurNaam, size_t breedte, size_t hoogte, bool herhaalS = false, bool herhaalT = false, bool mipmap = false, GLenum internalFormat = GL_RGBA, void * data = nullptr,	GLenum dataFormat = GL_RGBA, 	GLenum dataType = GL_UNSIGNED_BYTE);
	glm::uvec2	textuurGrootte(		const std::string & textuurNaam) { return _textuurGroottes.at(textuurNaam); }
	GLuint		textuurId(			const std::string & textuurNaam) { return _texturen.at(textuurNaam);		}

	void		doeRekenVerwerker(const std::string & verwerker, glm::uvec3 groepGroottes, std::function<void()> renderVoorbereiding);

	///Kan gebruikt worden door subklasses om nog extra dingen voor te bereiden
	virtual void extraVoorbereidingen(GLuint /*programma*/) { }
	
protected:
	float					_schermVerhouding = 16.0f / 9.0f;
	std::string				_naam;
	GLuint					_huidigProgramma;

private:
	GLuint	slaShaderOp(const std::string & naam, GLuint shaderProgramma);

	GLFWwindow 			*	_glfwScherm = nullptr;

	std::map<std::string, GLuint>					_shaderProgrammas,
													_texturen;
	std::map<std::string, glm::uvec2>				_textuurGroottes;
	std::set<std::string>							_3dTexturen,
													_1dTexturen;
	static std::map<GLFWwindow *, weergaveScherm*>	_schermen;

	static toetsVerwerkerFunc _eigenVerwerker;
};