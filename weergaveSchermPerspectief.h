#pragma once
#include "weergaveSchermVierkant.h"

class weergaveSchermPerspectief : public weergaveSchermVierkant
{
public:
		weergaveSchermPerspectief(std::string Naam = "weergaveSchermPerspectief", size_t W = 1280, size_t H = 720, size_t multiSamples = 1, bool hoofdloos = false);
		~weergaveSchermPerspectief();

	void		herberekenProjectie()	{ _projectie = glm::perspective(_fovYScale, _schermVerhouding, _dichtbij, _verWeg); };
	void		herberekenModelZicht();

	glm::mat4	projectie()					{ return _projectie; 	}
	glm::mat4	modelZicht()				{ return _modelZicht; 	}
	glm::mat4	modelView()					{ return _modelZicht; 	}

	void		zetModelZicht(glm::mat4 modelZicht);
void		setModelView(glm::mat4 modelView) { zetModelZicht(modelView); }
	void 		toetsVerwerker(int key, int scancode, int action, int mods) override;

	///Roteert de camera om het middelpunt (orbit): yaw om de Y-as, pitch om de
	///X-as. De pitch wordt geklemd zodat de camera niet over de polen heen slaat.
	void		roteer(float dyaw, float dpitch);
	///Zoomt in (delta > 0) of uit (delta < 0) op het middelpunt: verandert de
	///kijkafstand (vertaaling langs de kijkas), geklemd binnen redelijke grenzen.
	void		zoom(float delta);

	void 		extraVoorbereidingen(WGPURenderPipeline programma) override;

private:
	float		_fovYScale			= FPI * 0.5f,
				_dichtbij			= 0.01f,
				_verWeg				= 100.0f;
		
	glm::mat4	_projectie			= glm::mat4(1.0f), 
				_modelZicht			= glm::mat4(1.0f),
				_transInvMV 		= glm::mat4(1.0f);

	glm::vec3	_verplaatsing		= glm::vec3(0.0f, 0.0f, -2.0f);
	glm::vec2	_verdraaiing		= glm::vec2(-1.2f, -0.2f); //kijk richting Valles Marineris op de MOLA-kaart

};