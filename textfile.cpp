#include "textfile.h"
#include <vector>
#include <cstdio>
#include <cstring>
#include <unistd.h>

bool checkvoorshadercompileerfout(GLuint shader,const char* naam)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);
	if(isCompiled == GL_FALSE)
	{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
	 
			//The maxLength includes the NULL character
			std::vector<char> errorLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
	 
			printf("Shadercompilatie van %s is gefaald..\nCheck dit: %s\n",naam,errorLog.data());
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

GLuint createshaderobject(std::string shaderfilename, GLenum shadertype, std::string shaderdirectory)
{
	std::string shaderfilenameinshadersdirectory = shaderdirectory + shaderfilename;
	
	//printf((GetShaderTypeString(shadertype) + "-file: " + shaderfilenameinshadersdirectory + "\n").c_str());
	
	GLuint shaderobject = glCreateShader(shadertype);

	char * shadersource = textFileRead(shaderfilenameinshadersdirectory.c_str());

	char const ** shadersourcefakearray = const_cast<char const **>(&shadersource);
	
	glShaderSource(shaderobject, 1, shadersourcefakearray, NULL);

	free(shadersource);
	
	//printf("Compiling %s...\n",shaderfilename.c_str());
	glCompileShader(shaderobject);
	
	if(!checkvoorshadercompileerfout(shaderobject, shaderfilenameinshadersdirectory.c_str())) 
		exit(1);

	return shaderobject;
}

GLuint creategeomshader(char* vertshaderfilename, char* fragshaderfilename, char* geomshaderfilename, std::string Directory)
{
	GLuint vertshaderobject = createshaderobject(vertshaderfilename, GL_VERTEX_SHADER,		Directory);
	GLuint fragshaderobject = createshaderobject(fragshaderfilename, GL_FRAGMENT_SHADER,	Directory);
	GLuint geomshaderobject = createshaderobject(geomshaderfilename, GL_GEOMETRY_SHADER,	Directory);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);
	glAttachShader(prog, geomshaderobject);

	glLinkProgram(prog);
	return prog;
}

GLuint createshader(char* vertshaderfilename, char* fragshaderfilename, std::string Directory)
{
	GLuint vertshaderobject = createshaderobject(vertshaderfilename, GL_VERTEX_SHADER,	Directory);
	GLuint fragshaderobject = createshaderobject(fragshaderfilename, GL_FRAGMENT_SHADER,	Directory);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, fragshaderobject);
	glAttachShader(prog, vertshaderobject);

	glLinkProgram(prog);
	return prog;
}



GLuint createcomputeshader(char* shaderfilename, std::string Directory)
{
	GLuint compshaderobject = createshaderobject(shaderfilename, GL_COMPUTE_SHADER,	Directory);

	GLuint prog = glCreateProgram();
	
	glAttachShader(prog, compshaderobject);

	glLinkProgram(prog);
	return prog;
}


//These two functions should probably be rewritten to use streams instead of this old-fashioned crap
char *textFileRead(const char *fn) {

	FILE *fp;
	char *content = NULL;

	int f,count;
	f = open(fn, O_RDONLY);

	count = lseek(f, 0, SEEK_END);

	close(f);

	if (fn != NULL) {
		fp = fopen(fn,"rt");

		if (fp != NULL) {


			if (count > 0) {
				content = (char *)malloc(sizeof(char) * (count+1));
				count = fread(content,sizeof(char),count,fp);
				content[count] = '\0';
			}
			fclose(fp);
		}
	}
	return content;
}

int textFileWrite(char *fn, char *s) {

	FILE *fp;
	int status = 0;

	if (fn != NULL) {
		fp = fopen(fn,"w");

		if (fp != NULL) {
			
			if (fwrite(s,sizeof(char),strlen(s),fp) == strlen(s))
				status = 1;
			fclose(fp);
		}
	}
	return(status);
}







