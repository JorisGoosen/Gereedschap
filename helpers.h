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

GLuint 		createshaderobject(			const std::string &  shaderfilename, 	GLenum shadertype);
GLuint 		createcomputeshader(		const std::string &  shaderfilename);
GLuint 		createshader(				const std::string &  vertshaderfilename, 	const std::string &  fragshaderfilename);
GLuint 		creategeomshader(			const std::string &  vertshaderfilename, 	const std::string &  fragshaderfilename, const std::string &  geomshaderfilename);
GLuint 		createtesselationshader(	const std::string &  vertshaderfilename, 	const std::string &  fragshaderfilename, const std::string &  tessEvalFilename, const std::string &  tessCtrlFilename = "");

//willekeurig is een beetje te lang
		glm::vec3	randomVec3();
inline 	glm::vec3	randomVec3Z() { return glm::vec3(-1.0f) + (2.0f * randomVec3()); }

png_byte *	laadPNG(const std::string & bestandsnaam, size_t & width, size_t & height, size_t & kanalen);