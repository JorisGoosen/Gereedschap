#include "geodesisch.h"
#include <random>



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
	//Een vertex attrib pointer kan maar max 4 velden hebben (ivec4) maar das niet erg.
	//Configuratie van [{{#buren, buur0, buur1, buur3}, {buur4, buur5, ( ? | buur6), ?}}, ...]
	//Dus twee aparte attribpointers maar zelfde buffer met stride 4
	burenAlsEigenschapWijzers();
	
	//Nu nog twee buffers toevoegen waarin ik kan gaan rekenen en als punteigenschapwijzers kan gebruiken.
	maakPingPongOpslagen();

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
			glm::vec3 	A = _punten->ggvPunt3(a),
						B = _punten->ggvPunt3(b),
						C = glm::normalize((A + B));

			size_t  c = _punten->ggvPuntErbij(C);

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

	

	_punten->spoel();

	std::cout << "Na het verdelen en heersen blijken er #" << _punten->grootte() << " punten te zijn!" << std::endl;
}

void Geodesisch::maakLijstBuren()
{
	_buren.resize(_punten->grootte());

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

void Geodesisch::burenAlsEigenschapWijzers()
{
	_eigenschappen.reserve(_buren.size() * 8); //De grootte is eigenlijk _buren.size() * 6 - 12 maar ik denk dat het beter is voor de gpu om ze allebei een vec4 te geven, anders accepteert ie misschien wel, maar...
	
	std::random_device willekeur;

	for(const auto & buurt : _buren)
	{
		_eigenschappen.push_back(buurt.size());

		assert(buurt.size() == 5 || buurt.size() == 6);

		for(const auto & buur : buurt)
			_eigenschappen.push_back(buur);

		if(buurt.size() == 5)
			_eigenschappen.push_back(0);

		//0 = aantal, 1...6	buren, dat laat 7 over voor:
		_eigenschappen.push_back(willekeur()%3);

		assert(_eigenschappen.size() % 8 == 0);
	
	}

	_reeks->reeksOpslagErbij(4, _eigenschappen, { stapWijzer(1, 8, 0), stapWijzer(2, 8, 4) } );

	
}


void Geodesisch::maakPingPongOpslagen()
{
	std::vector<float> pingPong;
	pingPong.reserve(_buren.size() * 4); //Per element 1 vec4 voor nu. misschien later meer


	std::random_device rd;  //Wordt gebruikt om het zaadje te planten
    std::mt19937 gen(rd()); //Standaard mersenne_twister_engine gezaaid met rd()
    
	//std::uniform_real_distribution<> dis(0.0, 1.0);
	std::normal_distribution<> dis(0.5, 0.6);

	for(const auto & buurt : _buren)
		for(size_t i=0; i<4; i++)
			pingPong.push_back(1.0 - dis(gen));

	//We gebruiken twee keer dezelfde data, want het wordt gekopieerd en de een wordt straks toch overschreven door de ander maar zo is iig de goeie grootte.
	_pingPongOpslag[0] = _reeks->reeksOpslagErbij(4, pingPong, 3);
	_pingPongOpslag[1] = _reeks->reeksOpslagErbij(4, pingPong, 4);

	//We kunnen _pingPongOpslag straks wel allebei gebruiken om een SSB aan te binden en een compute shader tegenaan te gooien.
	//Voor nu maar gewoon zo houden denk ik.
}