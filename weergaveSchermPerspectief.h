#pragma once
#include "weergaveScherm.h"

class weergaveSchermPerspectief : public weergaveScherm
{
public:
		weergaveSchermPerspectief(std::string Naam = "weergaveSchermPerspectief", size_t W = 1280, size_t H = 720, size_t multiSamples = 1);
		~weergaveSchermPerspectief();

	void		herberekenProjectie()	{ _projectie = glm::perspective(_fovYScale, _schermVerhouding, _dichtbij, _verWeg); };
	void		herberekenModelZicht();

	glm::mat4	projectie()					{ return _projectie; 	}
	glm::mat4	modelZicht()				{ return _modelZicht; 	}

	void		zetModelZicht(glm::mat4 modelZicht);
	void 		toetsVerwerker(int key, int scancode, int action, int mods) override;

	void 		extraVoorbereidingen(GLuint programma) override;

private:
	float		_fovYScale			= FPI * 0.5f,
				_dichtbij			= 0.01f,
				_verWeg				= 100.0f;
		
	glm::mat4	_projectie			= glm::mat4(1.0f), 
				_modelZicht			= glm::mat4(1.0f),
				_transInvMV 		= glm::mat4(1.0f);

	glm::vec3	_verplaatsing		= glm::vec3(0.0f, 0.0f, -2.0f);
	glm::vec2	_verdraaiing		= glm::vec2(0.0f, 0.6f);

};