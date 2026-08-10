#include "vierkantRooster.h"
#include <set>
#include <iostream>

using namespace glm;

void vierkantRooster::tekenJezelf() const
{
	//WebGPU kent geen GL_QUADS; elk vierkant wordt daarom in twee driehoeken geknipt
	std::vector<glm::uint32> driehoeken;
	driehoeken.reserve(_vierkanten.size() / 4 * 6);

	for(size_t i = 0; i < _vierkanten.size(); i += 4)
	{
		uint32_t a = _vierkanten[i + 0],
				 b = _vierkanten[i + 1],
				 c = _vierkanten[i + 2],
				 d = _vierkanten[i + 3];

		driehoeken.push_back(a);
		driehoeken.push_back(b);
		driehoeken.push_back(c);

		driehoeken.push_back(a);
		driehoeken.push_back(c);
		driehoeken.push_back(d);
	}

	_reeks->zetIndexGegevens(driehoeken);
	_reeks->zetTopologie(WGPUPrimitiveTopology_TriangleList);
	_reeks->tekenGeïndexeerd();
}

void vierkantRooster::tekenJezelfPatchy() const
{
	werpOnondersteund("vierkantRooster::tekenJezelfPatchy (tessellation bestaat niet in WebGPU)");
}



vierkantRooster::vierkantRooster(size_t breedte, size_t hoogte, float schaling) : _breedte(breedte), _hoogte(hoogte), _schaling(schaling)
{
	_reeks  	= new wrgvOpslag			();
	_punten		= new wrgvOnderOpslag<float>(	3, _reeks, 0);
	_texturen	= new wrgvOnderOpslag<float>(	2, _reeks, 1);

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
			_punten		->ggvPuntErbij(glm::highp_vec3((-1.0f + (2.0f * xStap * x)) * _schaling, 0.0f, (-1.0f + (2.0f * yStap * y)) * _schaling));
			_texturen	->ggvPuntErbij(glm::highp_vec2(xStap * x, yStap * y));

			if(x < _breedte && y < _hoogte)// && x % 2 ==0 && y %2 == 1)
			{
				_vierkanten.push_back(plek(x, 	y));
				_vierkanten.push_back(plek(x, 	y+1));				
				_vierkanten.push_back(plek(x+1, y+1));
				_vierkanten.push_back(plek(x+1, y));				
			}
		}

	_punten		->spoel();
	_texturen	->spoel();
}
