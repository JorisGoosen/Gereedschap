#pragma once
#include "RenderScherm.h"

class RenderSchermPerspective : public RenderScherm
{
public:
		RenderSchermPerspective(std::string Naam = "RenderSchermPerspective", size_t W = 1280, size_t H = 720);
		~RenderSchermPerspective();

	void		RecalculateProjection()	{ _projection = glm::perspective(_fovYScale, _aspectRatio, _nearDistance, _farDistance); };
	//void		RecalculateModelView()	{ RecalculateModelView(true); }
	//void		RecalculateModelView(bool IncorporateTranslation);

	glm::mat4	projection()			{ return _projection; 	}
	glm::mat4	modelView()				{ return _modelView; 	}

	void		setModelView(glm::mat4 modelView) { _modelView = modelView; }
	//void keyHandler(int key, int scancode, int action, int mods) override;

private:
	float		_fovYScale			= FPI * 0.25f,
				_nearDistance		= 0.001f,
				_farDistance		= 100.0f;
		
	glm::mat4	_projection, 
				_modelView;
};