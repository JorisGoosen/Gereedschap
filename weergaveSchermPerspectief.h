#pragma once
#include "weergaveScherm.h"

class weergaveSchermPerspectief : public weergaveScherm
{
public:
		weergaveSchermPerspectief(std::string Naam = "weergaveSchermPerspectief", size_t W = 1280, size_t H = 720, size_t multiSamples = 1);
		~weergaveSchermPerspectief();

	void		RecalculateProjection()	{ _projection = glm::perspective(_fovYScale, _aspectRatio, _nearDistance, _farDistance); };
	//void		RecalculateModelView()	{ RecalculateModelView(true); }
	//void		RecalculateModelView(bool IncorporateTranslation);

	glm::mat4	projection()			{ return _projection; 	}
	glm::mat4	modelView()				{ return _modelView; 	}

	void		setModelView(glm::mat4 modelView);
	//void keyHandler(int key, int scancode, int action, int mods) override;

	void extraVoorbereidingen(GLuint programma) override;

private:
	float		_fovYScale			= FPI * 0.5f,
				_nearDistance		= 0.01f,
				_farDistance		= 100.0f;
		
	glm::mat4	_projection			= glm::mat4(1.0f), 
				_modelView			= glm::mat4(1.0f),
				_transInvMV 		= glm::mat4(1.0f);

};