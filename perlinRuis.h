#pragma once

#include "vrwrkrOpslagDing.h"
#include <algorithm>
#include <vector>
#include <random>
#include <list>


#define PERLIN_AANTAL_GRADIENTEN 1024

class perlinRuis
{
public:
    		            	perlinRuis();
							~perlinRuis();
			void			begin();
			void			zetKnooppunten(int gradientAdres, int herschikkingsAdres);		
    		glm::vec3   	gradient(glm::ivec3 loc);
    		float 			geefIniqoQuilesRuis(glm::vec3 x);

	static	perlinRuis	* 	dePerlin() { return _perlin; }
		
private:
	void		maakGradienten();
	void		maakHerschikkingsTabel();
    int			vouwFunctie(glm::ivec3 ijk) { return _tabel[ _tabel[ _tabel[ ijk[0] ] + ijk[1] ] + ijk[2] ]; }

	std::random_device	_willekeur;  
    std::mt19937 		_bemonsteraar;

    vrwrkrOpslagDing<glm::vec3> *	_willekeurigeGradienten 	= nullptr;
    vrwrkrOpslagDing<int>		*	_herschikkingsTabel 		= nullptr;
	std::vector<glm::vec3> 			_gradienten;
	std::vector<int> 				_tabel;
		
	static perlinRuis 			*	_perlin;
};


