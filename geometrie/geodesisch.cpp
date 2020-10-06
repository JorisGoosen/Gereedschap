#include "geodesisch.h"



using namespace glm;

Geodesisch::Geodesisch(size_t onderverdelingen) : icosahedron(), _onderverdelingen(onderverdelingen)
{
	// icosahedron::genereer() wordt aangeroepen in Ico-constr
	//Nu hebben we in _punten _drieHk alles wat we nodig hebben
	//Dan moeten we alleen nog alle driehoeken verder onderverdelen
	verdeelEnHeers();

	//Daarna maken we een lijst van al deze driehoeken
	//Via deze lijst kunnen we per punt de buren bepalen, waarbij er 12 5 buren (de originele) hebben en de rest 6,
	maakLijstBuren();
	
	//Ook moet ieder punt een lijstje meekrijgen met de buren waar ie bij hoort (dit is ook erg handig voor het tekenen van genoemde polygoon)
	//Alsin, dat ze naast een coordinaat nog 5 of 6 indices als vertex attribuut heeft

	
	//Misschien is het ook een goed idee om naderhand eens alle punten te maken en te herordenen gebaseerd longitude en latitude zodat er evt beter gecached kan worden op de gpu
	
	//Dan een geometrische shader maken die een polygoon tekent per punt

}

void Geodesisch::verdeelEnHeers() 
{
	

	for(size_t onderverdeling=0; onderverdeling<_onderverdelingen; onderverdeling++)
	{

		std::vector<glm::uint32> drieHk = _drieHk;
		_drieHk.clear();

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
			glm::vec3 	A = _punten->GetDataPoint3(a),
						B = _punten->GetDataPoint3(b),
						C = glm::normalize((A + B));

			size_t  c = _punten->AddDataPoint(C);

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

			_drieHk.push_back(v0 );
			_drieHk.push_back(v01);
			_drieHk.push_back(v20);

			_drieHk.push_back(v1 );
			_drieHk.push_back(v12);
			_drieHk.push_back(v01);

			_drieHk.push_back(v2 );
			_drieHk.push_back(v20);
			_drieHk.push_back(v12);

			_drieHk.push_back(v01);
			_drieHk.push_back(v12);
			_drieHk.push_back(v20);
		}
	}

	

	_punten->Flush();

	std::cout << "Na het verdelen en heersen blijken er #" << _punten->Aantal() << " punten te zijn!" << std::endl;
}

void Geodesisch::maakLijstBuren()
{
	for(size_t i=0; i<_drieHk.size(); i+=3)
	{
		size_t 	v0	= _drieHk[ i   ],
				v1	= _drieHk[ i+1 ],
				v2	= _drieHk[ i+2 ];

		_buren[ v0 ].insert( v1 );
		_buren[ v0 ].insert( v2 );

		_buren[ v1 ].insert( v0 );
		_buren[ v1 ].insert( v2 );

		_buren[ v2 ].insert( v0 );
		_buren[ v2 ].insert( v1 );				
	}

	
}







