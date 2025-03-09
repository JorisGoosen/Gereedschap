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

		_grootte = gegevens.size();
	}

	void	zetKnooppunt(GLuint knooppunt)
	{
		glBindBuffer(		GL_SHADER_STORAGE_BUFFER, _opslag);
		glBindBufferBase(	GL_SHADER_STORAGE_BUFFER, knooppunt, _opslag);
	}

	void bindPuntReeks()
	{
		glBindVertexArray(_reeksOpslag);
		glErrorToConsole("vrwrkrOpslagDing::bindPuntReeks() BindVertexArray: ");

	}

	void 	maakReeksOpslag(int array = 0)
	{
		//Ik neem voor het gemak maar aan dat dit geladen moet worden als vec4 en anders jammer dan
		assert(sizeof(Soort) == sizeof(float) * 4);

		glCreateVertexArrays(1, &_reeksOpslag);
		glErrorToConsole("glCreateVertexArrays in vrwrkrOpslagDing::maakReeksOpslag: ");

		glBindVertexArray(_reeksOpslag);

		glBindBuffer(GL_ARRAY_BUFFER, _opslag);
		glErrorToConsole("glBindBuffer in vrwrkrOpslagDing::maakReeksOpslag: ");

		glVertexAttribPointer(0, 4, GL_FLOAT, false, 0, 0);
		glErrorToConsole("glVertexAttribPointer in vrwrkrOpslagDing::maakReeksOpslag: ");

		glEnableVertexAttribArray(array);
		glErrorToConsole("glEnableVertexAttribArray in vrwrkrOpslagDing::maakReeksOpslag: ");

		//ontbind:
	//	glBindBuffer(GL_ARRAY_BUFFER, 0);
    //	glBindVertexArray(0);
	}


	GLuint grootte()
	{
		return _grootte;
	}

private:
	GLuint 	_opslag,
			_reeksOpslag, ///< Voor als je deze buffer wilt kunnen weergeven als punten, moet wel worden aangemaakt eerst
			_grootte;
};