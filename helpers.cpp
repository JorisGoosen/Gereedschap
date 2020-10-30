#include "helpers.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <png.h>
#include <exception>

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

	throw std::runtime_error("...");
}


bool checkvoorshadercompileerfout(GLuint shader, const std::string & naam)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
	 
			//The maxLength includes the nullptr character
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


GLuint _maakShaderObject(const std::string & shaderBestandsnaam, GLenum shadertype)
{
	GLuint shaderobject = glCreateShader(shadertype);

	std::string shaderSource = textFileRead(shaderBestandsnaam);

	//The following seems a bit weird no?
	char const * pointerToCStr = shaderSource.c_str();

	char const ** shadersourcefakearray = const_cast<char const **>(&pointerToCStr);
	
	glShaderSource(shaderobject, 1, shadersourcefakearray, nullptr);

	std::cout << "Compiling " << shaderBestandsnaam << "..." << std::endl;

	glCompileShader(shaderobject);
	
	if(!checkvoorshadercompileerfout(shaderobject, shaderBestandsnaam.c_str())) 
		exit(1);

	return shaderobject;
}

GLuint _maakGeometrieShader(const std::string & vertShaderBestandsnaam, const std::string & fragShaderBestandsnaam, const std::string & geomShaderBestandsnaam)
{
	GLuint vertshaderobject = _maakShaderObject(vertShaderBestandsnaam, GL_VERTEX_SHADER	);
	GLuint fragshaderobject = _maakShaderObject(fragShaderBestandsnaam, GL_FRAGMENT_SHADER	);
	GLuint geomshaderobject = _maakShaderObject(geomShaderBestandsnaam, GL_GEOMETRY_SHADER	);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);
	glAttachShader(prog, geomshaderobject);

	glLinkProgram(prog);
	return prog;
}

GLuint _maakVlakVerdelingShader(	const std::string & vertShaderBestandsnaam, 	const std::string & fragShaderBestandsnaam, 
								const std::string & vlakEvalBestandsnaam, 		const std::string & vlakCtrlBestandsnaam)
{
	GLuint vertshaderobject = _maakShaderObject(vertShaderBestandsnaam, 	GL_VERTEX_SHADER	);
	GLuint fragshaderobject = _maakShaderObject(fragShaderBestandsnaam, 	GL_FRAGMENT_SHADER	);
	GLuint tessEvalObject 	= _maakShaderObject(vlakEvalBestandsnaam, 		GL_TESS_EVALUATION_SHADER);

	GLuint tessCtrlObject	= vlakCtrlBestandsnaam == "" ? 0 : _maakShaderObject(vlakCtrlBestandsnaam, GL_TESS_CONTROL_SHADER);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);
	glAttachShader(prog, tessEvalObject);

	if(vlakCtrlBestandsnaam != "")
		glAttachShader(prog, tessCtrlObject);

	glLinkProgram(prog);
	return prog;
}



GLuint _maakShader(const std::string & vertShaderBestandsnaam, const std::string & fragShaderBestandsnaam)
{
	GLuint vertshaderobject = _maakShaderObject(vertShaderBestandsnaam, GL_VERTEX_SHADER	);
	GLuint fragshaderobject = _maakShaderObject(fragShaderBestandsnaam, GL_FRAGMENT_SHADER	);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);

	glLinkProgram(prog);
	return prog;
}



GLuint _maakBerekenShader(const std::string & shaderBestandsnaam)
{
	GLuint compshaderobject = _maakShaderObject(shaderBestandsnaam, GL_COMPUTE_SHADER);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, compshaderobject);

	glLinkProgram(prog);
	return prog;
}


glm::vec3 willekeurigeVec3()
{
	const float randMax = RAND_MAX;

	return glm::vec3(rand() / randMax, rand() / randMax, rand() / randMax);
}


png_byte *	laadPNG(const std::string & bestandsnaam, size_t & width, size_t & height, size_t & kanalen)
{
    png_image image;

	memset(&image, 0, (sizeof image));
	image.version = PNG_IMAGE_VERSION;

	if (png_image_begin_read_from_file(&image, bestandsnaam.c_str()))
	{
		png_bytep buffer = nullptr;
		image.format = PNG_FORMAT_RGBA;

		buffer = new png_byte[PNG_IMAGE_SIZE(image)];

		if (buffer != nullptr && png_image_finish_read(&image, nullptr, buffer, 0, nullptr) != 0)
		{
			width 	= image.width;
			height 	= image.height;
			kanalen	= 4;

			std::cout << "Plaatje '" << bestandsnaam << "' geladen en formaat: " << width << " X " << height << std::endl;

			return buffer;
		}

		delete buffer;
	}

	  return nullptr;  
}