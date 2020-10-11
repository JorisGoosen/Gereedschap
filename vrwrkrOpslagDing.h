#pragma once
#include "helpers.h"
#include <map>
#include <list>
#include <vector>
#include <algorithm>

template <class Soort>
class vrwrkrOpslagDing
{
public:
	vrwrkrOpslagDing(const std::vector<Soort> & gegevens, GLuint knooppunt = 0) 
	{
		
		glGenBuffers(1, & _opslag);

		glBindBuffer(		GL_SHADER_STORAGE_BUFFER, _opslag);
		glBufferData(		GL_SHADER_STORAGE_BUFFER, gegevens.size() * sizeof(Soort), gegevens.data(), GL_DYNAMIC_COPY);
		glBindBufferBase(	GL_SHADER_STORAGE_BUFFER, knooppunt, _opslag);
	}

	void	zetKnooppunt(GLuint knooppunt)
	{
		glBindBuffer(		GL_SHADER_STORAGE_BUFFER, _opslag);
		glBindBufferBase(	GL_SHADER_STORAGE_BUFFER, knooppunt, _opslag);
	}

private:
	GLuint _opslag;

};