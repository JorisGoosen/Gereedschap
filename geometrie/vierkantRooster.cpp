#include "vierkantRooster.h"
#include <set>
#include <iostream>

using namespace glm;

void vierkantRooster::tekenJezelf() const
{
	bindPuntReeks();
	glDrawElements(GL_QUADS, _vierkanten.size(), GL_UNSIGNED_INT, _vierkanten.data());
	glErrorToConsole("vierkantRooster::tekenJezelf(): ");
}

void vierkantRooster::tekenJezelfPatchy() const
{
	glPatchParameteri(GL_PATCH_VERTICES, 4);
	bindPuntReeks();
	glDrawElements(GL_PATCHES, _vierkanten.size(), GL_UNSIGNED_INT, _vierkanten.data());
	glErrorToConsole("vierkantRooster::tekenJezelfPatchy(): ");
}



vierkantRooster::vierkantRooster(size_t breedte, size_t hoogte) : _breedte(breedte), _hoogte(hoogte)
{
	_reeks  	= new wrgvOpslag			();
	_punten	= new wrgvOnderOpslag<float>(	3, _reeks, 0);

	glErrorToConsole("vierkantRooster::vierkantRooster(): ");

	genereer();
}

void vierkantRooster::genereer()
{
	const float xStap = 1.0 / _breedte,
				yStap = 1.0 / _hoogte;

	auto plek = [&](size_t x, size_t y) { return x + (y * (_breedte + 1)); };

	for(size_t y=0; y<=_hoogte; y++)
		for(size_t x=0; x<=_breedte; x++)
		{
			_punten->ggvPuntErbij(glm::highp_vec3(xStap * x, 0.0f, yStap * y));

			if(x < _breedte && y < _hoogte)// && x % 2 ==0 && y %2 == 1)
			{
				_vierkanten.push_back(plek(x, 	y));
				_vierkanten.push_back(plek(x+1, y));
				_vierkanten.push_back(plek(x+1, y+1));
				_vierkanten.push_back(plek(x, 	y+1));				
			}
		}

	_punten->spoel();
}
