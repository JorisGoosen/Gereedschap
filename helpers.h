#pragma once

#include <GL/glew.h>
#include <string>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "png.h"


void glErrorToConsole(const std::string & huidigeActie = "");

GLuint 		_maakShaderObject(			const std::string &  shaderBestandsnaam, 		GLenum shadertype);
GLuint 		_maakBerekenShader(			const std::string &  shaderBestandsnaam);
GLuint 		_maakShader(				const std::string &  vertShaderBestandsnaam, 	const std::string &  fragShaderBestandsnaam);
GLuint 		_maakGeometrieShader(		const std::string &  vertShaderBestandsnaam, 	const std::string &  fragShaderBestandsnaam, const std::string &  geomShaderBestandsnaam);
GLuint 		_maakVlakVerdelingShader(	const std::string &  vertShaderBestandsnaam, 	const std::string &  fragShaderBestandsnaam, const std::string &  vlakEvalBestandsnaam, const std::string &  vlakCtrlBestandsnaam = "");

		glm::vec3	willekeurigeVec3();
inline 	glm::vec3	willekeurigeVec3Z() { return glm::vec3(-1.0f) + (2.0f * willekeurigeVec3()); }

png_byte *	laadPNG(const std::string & bestandsnaam, size_t & width, size_t & height, size_t & kanalen);