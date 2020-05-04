#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <malloc.h>
#include <fcntl.h>
#include <time.h>


bool 	checkvoorshadercompileerfout(GLhandleARB shader,const char* naam);
char *	textFileRead(const char *fn);
int 	textFileWrite(char *fn, char *s);

std::string GetShaderTypeString(GLenum ShaderType);
GLuint 		createshaderobject(	std::string shaderfilename, GLenum shadertype, std::string shaderdirectory);
GLuint 		creategeomshader(	char* vertshaderfilename, 	char* fragshaderfilename, char* geomshaderfilename, std::string Directory = "../Shaders/");
GLuint 		createshader(		char* vertshaderfilename, 	char* fragshaderfilename, 							std::string Directory = "../Shaders/");
GLuint 		createcomputeshader(char* shaderfilename, 															std::string Directory = "../Shaders/");





