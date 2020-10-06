#include "geodesisch.h"
#include <stdexcept>
#include <map>
#include <iostream>


using namespace glm;

Geodesisch::Geodesisch(size_t onderverdelingen) : Icosahedron(), _onderverdelingen(onderverdelingen)
{
	// Icosahedron::genereer() wordt aangeroepen in Ico-constr
	//Nu hebben we in _icoPunten _icoDriehk alles wat we nodig hebben
	//Dan moeten we alleen nog alle driehoeken verder onderverdelen
	verdeelEnHeers();

	//Daarna maken we een lijst van al deze driehoeken
	//Via deze lijst kunnen we per punt de buren bepalen, waarbij er 12 5 buren (de originele) hebben en de rest 6,
	
	//Ook moet ieder punt een lijstje meekrijgen met de buren waar ie bij hoort (dit is ook erg handig voor het tekenen van genoemde polygoon)
	//Alsin, dat ze naast een coordinaat nog 5 of 6 indices als vertex attribuut heeft
	
	//Misschien is het ook een goed idee om naderhand eens alle punten te maken en te herordenen gebaseerd longitude en latitude zodat er evt beter gecached kan worden op de gpu
	
	//Dan een geometrische shader maken die een polygoon tekent per punt

}

void Geodesisch::verdeelEnHeers() 
{
	

	for(size_t onderverdeling=0; onderverdeling<_onderverdelingen; onderverdeling++)
	{

		std::vector<glm::uint32> drieHk = _icoDriehk;
		_icoDriehk.clear();

		if(drieHk.size() % 3 != 0)
			throw std::runtime_error("Driehoeken waren niet, nou, 3 hoeken... De grootte van de lijst was: " + std::to_string(drieHk.size()));

		std::map<size_t, std::map<size_t, size_t>> kindjes;

		auto vindKind = [&](size_t a, size_t b) -> size_t
		{
			if(a > b) 
				std::swap(a, b);

			if(kindjes.count(a) > 0 && kindjes[a].count(b) > 0)
				return kindjes[a][b];

			//Ok er was nog geen kind hier, laten we er ffkes eentje interpoleren op een bal
			glm::vec3 	A = _icoPunten->GetDataPoint3(a),
						B = _icoPunten->GetDataPoint3(b),
						C = glm::normalize((A + B));

			size_t  c = _icoPunten->AddDataPoint(C);

			kindjes[a][b] = c;

			return c;
		};

		for(size_t i=0; i<drieHk.size(); i+=3)
		{
			size_t 	v0	= drieHk[i  ],
					v1	= drieHk[i+1],
					v2	= drieHk[i+2],
					v01	= vindKind(v0, v1),
					v12 = vindKind(v1, v2),
					v20 = vindKind(v2, v0);

			_icoDriehk.push_back(v0 );
			_icoDriehk.push_back(v01);
			_icoDriehk.push_back(v20);

			_icoDriehk.push_back(v1 );
			_icoDriehk.push_back(v12);
			_icoDriehk.push_back(v01);

			_icoDriehk.push_back(v2 );
			_icoDriehk.push_back(v20);
			_icoDriehk.push_back(v12);

			_icoDriehk.push_back(v01);
			_icoDriehk.push_back(v12);
			_icoDriehk.push_back(v20);
		}
	}

	

	_icoPunten->Flush();

	std::cout << "Na het verdelen en heersen blijken er #" << _icoPunten->Aantal() << " punten te zijn!" << std::endl;
}