#include "Helpers.h"
#include <fstream>
#include <sstream>
#include <iostream>

void glErrorToConsole(const std::string & huidigeActie)
{
	GLenum Fout = glGetError();

	if(Fout == GL_NO_ERROR)
		return;

	if(huidigeActie != "")
		std::cout << huidigeActie << ": ";

	switch(Fout)
	{
	case GL_INVALID_FRAMEBUFFER_OPERATION:	std::cout << "GL_INVALID_FRAMEBUFFER_OPERATION!";	break;
	case GL_INVALID_ENUM:					std::cout << "GL_INVALID_ENUM!";					break;
	case GL_OUT_OF_MEMORY:					std::cout << "GL_OUT_OF_MEMORY!";					break;
	case GL_STACK_UNDERFLOW:				std::cout << "GL_STACK_UNDERFLOW!";					break;
	case GL_STACK_OVERFLOW:					std::cout << "GL_STACK_OVERFLOW!";					break;
	case GL_INVALID_VALUE:					std::cout << "GL_INVALID_VALUE!";					break;
	case GL_INVALID_OPERATION:				std::cout << "GL_INVALID_OPERATION!";				break;
	default:								std::cout << "Onbekende fout!";						break;
	}

	std::cout << std::endl;
}


bool checkvoorshadercompileerfout(GLuint shader, const std::string & naam)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
	 
			//The maxLength includes the NULL character
			std::string errorLog;
			errorLog.resize(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, errorLog.data());
	 
	 		std::cout << "Shadercompilatie van " << naam << " is gefaald..\nCheck dit: " <<  errorLog.data();
			glDeleteShader(shader); //Don't leak the shader.
			return false;
	}
	
	return true;
}

std::string GetShaderTypeString(GLenum ShaderType)
{
	switch(ShaderType)
	{
	case GL_COMPUTE_SHADER:			return "GL_COMPUTE_SHADER";
	case GL_VERTEX_SHADER:			return "GL_VERTEX_SHADER";		
	case GL_TESS_CONTROL_SHADER:	return "GL_TESS_CONTROL_SHADER";		
	case GL_TESS_EVALUATION_SHADER:	return "GL_TESS_EVALUATION_SHADER";
	case GL_GEOMETRY_SHADER:		return "GL_GEOMETRY_SHADER";		
	case GL_FRAGMENT_SHADER:		return "GL_FRAGMENT_SHADER";
	default:						return "Unknown" ;
	}
}


std::string textFileRead(const std::string & fileName) 
{
	std::ifstream leesShader(fileName);

	if(!leesShader.is_open())
		throw std::runtime_error("Het openen van de shader " + fileName + " is helaas niet gelukt...");
	
	std::stringstream touw;

	touw << leesShader.rdbuf();

	return touw.str();
}


GLuint createshaderobject(const std::string & shaderfilename, GLenum shadertype)
{
	GLuint shaderobject = glCreateShader(shadertype);

	std::string shaderSource = textFileRead(shaderfilename);

	//The following seems a bit weird no?
	char const * pointerToCStr = shaderSource.c_str();

	char const ** shadersourcefakearray = const_cast<char const **>(&pointerToCStr);
	
	glShaderSource(shaderobject, 1, shadersourcefakearray, NULL);

	std::cout << "Compiling " << shaderfilename << "..." << std::endl;

	glCompileShader(shaderobject);
	
	if(!checkvoorshadercompileerfout(shaderobject, shaderfilename.c_str())) 
		exit(1);

	return shaderobject;
}

GLuint creategeomshader(const std::string & vertshaderfilename, const std::string & fragshaderfilename, const std::string & geomshaderfilename)
{
	GLuint vertshaderobject = createshaderobject(vertshaderfilename, GL_VERTEX_SHADER	);
	GLuint fragshaderobject = createshaderobject(fragshaderfilename, GL_FRAGMENT_SHADER	);
	GLuint geomshaderobject = createshaderobject(geomshaderfilename, GL_GEOMETRY_SHADER	);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);
	glAttachShader(prog, geomshaderobject);

	glLinkProgram(prog);
	return prog;
}

GLuint createtesselationshader(	const std::string & vertshaderfilename, 	const std::string & fragshaderfilename, 
								const std::string & tessEvalFilename, 		const std::string & tessCtrlFilename)
{
	GLuint vertshaderobject = createshaderobject(vertshaderfilename, 	GL_VERTEX_SHADER	);
	GLuint fragshaderobject = createshaderobject(fragshaderfilename, 	GL_FRAGMENT_SHADER	);
	GLuint tessEvalObject 	= createshaderobject(tessEvalFilename, 		GL_TESS_EVALUATION_SHADER);

	GLuint tessCtrlObject	= tessCtrlFilename == "" ? 0 : createshaderobject(tessEvalFilename, GL_TESS_CONTROL_SHADER);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);
	glAttachShader(prog, tessEvalObject);

	if(tessCtrlFilename != "")
		glAttachShader(prog, tessCtrlObject);

	glLinkProgram(prog);
	return prog;
}



GLuint createshader(const std::string & vertshaderfilename, const std::string & fragshaderfilename)
{
	GLuint vertshaderobject = createshaderobject(vertshaderfilename, GL_VERTEX_SHADER	);
	GLuint fragshaderobject = createshaderobject(fragshaderfilename, GL_FRAGMENT_SHADER	);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);

	glLinkProgram(prog);
	return prog;
}



GLuint createcomputeshader(const std::string & shaderfilename)
{
	GLuint compshaderobject = createshaderobject(shaderfilename, GL_COMPUTE_SHADER);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, compshaderobject);

	glLinkProgram(prog);
	return prog;
}


glm::vec3 randomVec3()
{
	const float randMax = RAND_MAX;

	return glm::vec3(rand() / randMax, rand() / randMax, rand() / randMax);
}