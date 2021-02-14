#include "weergaveSchermVierkant.h"
#include <iostream>

using namespace glm;


weergaveSchermVierkant::weergaveSchermVierkant(std::string Naam, size_t W, size_t H, bool texVanafNul) 
: weergaveScherm(Naam, W, H), _texVanafNul(texVanafNul)
{}

weergaveSchermVierkant::~weergaveSchermVierkant()
{
	delete _reeks;
	delete _punten;
	delete _tex;
	
	_reeks 	= nullptr;
	_punten = nullptr;
	_tex 	= nullptr;
}

void weergaveSchermVierkant::initQuad()
{
	if(_reeks != nullptr)
		throw std::runtime_error("InitQuad dubbel gedaan..\n");

	_reeks = new wrgvOpslag();
	
	_punten = new wrgvOnderOpslag<float>(2, _reeks, 0);
	_punten->ggvPuntErbij(vec2(-1.0f, -1.0f));
	_punten->ggvPuntErbij(vec2( 1.0f, -1.0f));
	_punten->ggvPuntErbij(vec2( 1.0f,  1.0f));
	_punten->ggvPuntErbij(vec2(-1.0f,  1.0f));
	
	_tex = new wrgvOnderOpslag<float>(2, _reeks, 1);
	const float minus = _texVanafNul ? 0.0f : -1.0;
	_tex->ggvPuntErbij(vec2(minus, minus));
	_tex->ggvPuntErbij(vec2( 1.0f, minus));
	_tex->ggvPuntErbij(vec2( 1.0f,  1.0f));
	_tex->ggvPuntErbij(vec2(minus,  1.0f));

	_punten->spoel();
	_tex->spoel();
}

void weergaveSchermVierkant::geefWeer()
{
	if(!_reeks)
		initQuad();

	glDisable(GL_DEPTH_TEST);
	unsigned int indices[] = {1, 2, 0, 3};

	_reeks->bindPuntReeks();	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, indices);
	glErrorToConsole("weergaveSchermVierkant::geefWeer(): ");
}