#pragma once
#include <GL/glew.h>
#include <string>

GLuint 		createshaderobject(	const std::string &  shaderfilename, 	GLenum shadertype);
GLuint 		creategeomshader(	const std::string &  vertshaderfilename, 	const std::string &  fragshaderfilename, const std::string &  geomshaderfilename);
GLuint 		createshader(		const std::string &  vertshaderfilename, 	const std::string &  fragshaderfilename);
GLuint 		createcomputeshader(const std::string &  shaderfilename);





