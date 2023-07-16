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

	void bindPuntReeks()
	{
		glBindVertexArray(_reeksOpslag);
		glErrorToConsole("vrwrkrOpslagDing::bindPuntReeks(): ");
	}

	void 	maakReeksOpslag()
	{
		//Ik neem voor het gemak maar aan dat dit geladen moet worden als vec4 en anders jammer dan
		assert(sizeof(Soort) == sizeof(float) * 4);

		glCreateVertexArrays(1, &_reeksOpslag);
		glErrorToConsole("glCreateVertexArrays in vrwrkrOpslagDing::maakReeksOpslag: ");

		bindPuntReeks();

		glBindBuffer(GL_ARRAY_BUFFER, _opslag);

		glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
		glEnableVertexAttribArray(0);

		//ontbind:
		glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glBindVertexArray(0);
	}

private:
	GLuint 	_opslag,
			_reeksOpslag; ///< Voor als je deze buffer wilt kunnen weergeven als punten, moet wel worden aangemaakt eerst

};