#include "icosahedron.h"
#include <set>
#include <iostream>
#include <array>

using namespace glm;

bool lijn::heeftPunt(unsigned int p) const	
{
	return a == p || b == p; 
}

bool lijn::vormtDriehoek(const lijn & deEen, const lijn & deAnder) const 
{
	if(this->operator==(deEen) || this->operator==(deAnder) || deEen == deAnder)
		return false;

	std::set<glm::uint32> setje ({a, b, deEen.a, deEen.b, deAnder.a, deAnder.b});

	return setje.size() == 3;
}
 
glm::ivec3 lijn::geefDriehoek(const lijn & deEen, const lijn & deAnder) const
{
	if(!vormtDriehoek(deEen, deAnder))
		return {0, 0, 0};

	std::set<glm::uint32> setje ({a, b, deEen.a, deEen.b, deAnder.a, deAnder.b});
	std::vector<glm::uint32> vecje(setje.begin(), setje.end());
	
	return { vecje[0], vecje[1], vecje[2] };
}
	
glm::ivec3 	lijn::geefGeorienteerdeDriehoek(const lijn & deEen, const lijn & deAnder, const wrgvOnderOpslag<float> * punten) const
{
	glm::ivec3 vecje(geefDriehoek(deEen, deAnder));

	glm::vec3	a = punten->ggvPunt3(vecje[0]), 
				b = punten->ggvPunt3(vecje[1]), 
				c = punten->ggvPunt3(vecje[2]);

	// We weten dat het centrum van de bol op 0,0,0 is. Dus als de normaal enigzins dezelfde kant op wijst is het goed. en anders wisselen we twee punten om.
	glm::vec3	been0 	= a - b,
				been1 	= c - b,
				normaal = glm::normalize(glm::cross(been0, been1)),
				straal 	= glm::normalize(a + b + c);

	if(glm::dot(straal, normaal) < 0) return vecje;

	std::swap(vecje[1], vecje[2]);

	return vecje;
}

void icosahedron::tekenJezelf() const
{
	bindPuntReeks();
	glDrawElements(GL_TRIANGLES, _drieHk.size(), GL_UNSIGNED_INT, _drieHk.data());
	glErrorToConsole("icosahedron::tekenJezelf(): ");
}

void icosahedron::tekenJezelfPatchy() const
{
	glPatchParameteri(GL_PATCH_VERTICES, 3);
	bindPuntReeks();
	glDrawElements(GL_PATCHES, _drieHk.size(), GL_UNSIGNED_INT, _drieHk.data());
	glErrorToConsole("icosahedron::tekenJezelfPatchy(): ");
}



icosahedron::icosahedron()
{
	_reeks  	= new wrgvOpslag			();
	_punten	= new wrgvOnderOpslag<float>(	3, _reeks, 0);

	glErrorToConsole("icosahedron::icosahedron(): ");

	genereer();
}

//#define DEBUGTRIANGLE

void icosahedron::genereer()
{
#ifdef DEBUGTRIANGLE
	_punten->ggvPuntErbij( vec3(-1.0f, 0.0f, 0.0f));
	_punten->ggvPuntErbij( vec3( 1.0f, 0.0f, 0.0f));
	_punten->ggvPuntErbij( vec3( 0.0f, 1.0f, 0.0f));

	_punten->spoel();
	
	_drieHk.push_back(0);
	_drieHk.push_back(1);
	_drieHk.push_back(2);

	glErrorToConsole("icosahedron::genereer: ");

	return;
#else
	float tao = 1.61803399;
	
	_punten->ggvPuntErbij( glm::normalize( vec3( -tao,	 1.0f,	 0.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(  tao,	 1.0f,	 0.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3( -tao,	-1.0f,	 0.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(  tao,	-1.0f,	 0.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	 0.0f,	-tao,	 1.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	 0.0f,	 tao,	 1.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	 0.0f,	-tao,	-1.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	 0.0f,	 tao,	-1.0f ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	 1.0f,	 0.0f,	-tao  ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	 1.0f,	 0.0f,	 tao  ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	-1.0f,	 0.0f,	-tao  ) ) );
	_punten->ggvPuntErbij( glm::normalize( vec3(	-1.0f,	 0.0f,	 tao  ) ) );

	_punten->spoel();

	std::array<lijn, ICOSAHEDRON_LIJNEN> _lijnen;
	
	_lijnen[29].a= 0;	_lijnen[29].b = 2;
	_lijnen[28].a= 1;	_lijnen[28].b = 3;
	_lijnen[23].a= 8;	_lijnen[23].b = 10;
	_lijnen[18].a= 9;	_lijnen[18].b = 11;
	_lijnen[9].a = 4;	_lijnen[9].b  = 6;
	_lijnen[0].a = 5;	_lijnen[0].b  = 7;

	_lijnen[1].a  = 0;	_lijnen[1].b  = 5;
	_lijnen[2].a  = 0;	_lijnen[2].b  = 7;
	_lijnen[3].a  = 0;	_lijnen[3].b  = 10;
	_lijnen[4].a  = 0;	_lijnen[4].b  = 11;
	_lijnen[5].a  = 1;	_lijnen[5].b  = 5;
	_lijnen[6].a  = 1;	_lijnen[6].b  = 7;
	_lijnen[7].a  = 1;	_lijnen[7].b  = 8;
	_lijnen[8].a  = 1;	_lijnen[8].b  = 9;
	_lijnen[10].a = 2;	_lijnen[10].b = 4;
	_lijnen[11].a = 2;	_lijnen[11].b = 6;
	_lijnen[12].a = 2;	_lijnen[12].b = 10;
	_lijnen[13].a = 2;	_lijnen[13].b = 11;
	_lijnen[14].a = 3;	_lijnen[14].b = 4;
	_lijnen[15].a = 3;	_lijnen[15].b = 6;
	_lijnen[16].a = 3;	_lijnen[16].b = 8;
	_lijnen[17].a = 3;	_lijnen[17].b = 9;
	_lijnen[19].a = 4;	_lijnen[19].b = 9;
	_lijnen[20].a = 4;	_lijnen[20].b = 11;
	_lijnen[21].a = 5;	_lijnen[21].b = 9;
	_lijnen[22].a = 5;	_lijnen[22].b = 11;
	_lijnen[24].a = 6;	_lijnen[24].b = 8;
	_lijnen[25].a = 6;	_lijnen[25].b = 10;
	_lijnen[26].a = 7;	_lijnen[26].b = 8;
	_lijnen[27].a = 7;	_lijnen[27].b = 10;

	for(size_t a=0; a<_lijnen.size() - 2; a++)
		for(size_t b=a+1; b<_lijnen.size() - 1; b++)
			for(size_t c=b+1; c<_lijnen.size(); c++)
				if(_lijnen[a].vormtDriehoek(_lijnen[b], _lijnen[c]))
				{
					glm::ivec3 driehoek = _lijnen[a].geefGeorienteerdeDriehoek(_lijnen[b], _lijnen[c], _punten);
					_drieHk.push_back(driehoek.x);
					_drieHk.push_back(driehoek.y);
					_drieHk.push_back(driehoek.z);
				}

	//std::cout << "Verwerken van icosahedron resulteerde in " << _drieHk.size() / 3 << " driehoeken en werden er " << ICOSAHEDRON_VLAKKEN << " verwacht!" << std::endl;
	#endif
}
