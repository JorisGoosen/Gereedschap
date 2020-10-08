#pragma once
#include "weergaveScherm.h"

class weergaveSchermVierkant : public weergaveScherm
{
public:
		weergaveSchermVierkant(std::string Naam = "weergaveSchermVierkant", size_t W = 1280, size_t H = 720);
		~weergaveSchermVierkant();

	void 		geefWeer();


	//void keyHandler(int key, int scancode, int action, int mods) override;

private:
	void		initQuad();
	
	wrgvOpslag 	*_reeks		= nullptr;
	wrgvOnderOpslag<float>	*_punten		= nullptr,
							*_tex		= nullptr;
};