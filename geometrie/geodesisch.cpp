#include "geodesisch.h"
#include <random>

geodesisch::geodesisch(size_t onderverdelingen) : icosahedron(), _onderverdelingen(onderverdelingen)
{
	// icosahedron::genereer() wordt aangeroepen in Ico-constr
	//Nu hebben we in _punten _drieHk alles wat we nodig hebben
	//Dan moeten we alleen nog alle driehoeken verder onderverdelen
	verdeelEnHeers();

	//Misschien is het ook een goed idee om naderhand eens alle punten te maken en te herordenen gebaseerd longitude en latitude zodat er evt beter gecached kan worden op de gpu
	ordenPunten();
}

void geodesisch::verdeelEnHeers() 
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

void geodesisch::ordenPunten()
{
	using namespace glm;

	//Eerst bepalen welke lengte- en breedtegraden alle punten hebben.
	std::map<size_t, vec2> adresLengteBreedte;

	//Deze mapping zou er in principe voor moeten zorgen dat ik goed kan samplen uit een cylindrisch geprojecteerd planeetweergave (zoals MOLA voor Mars)
	auto berekenLengteBreedte = [&](size_t adres) -> vec2
	{
		//Alle posities zijn al genormaliseerd
		vec3 	positie		= _punten->ggvPunt3(adres);
		float 	latitude 	= (positie.y + 1.0) * 0.5;
		vec2	horizontaal	= positie.xz();

		//Is het een pool?
		if(length(horizontaal) == 0)	horizontaal = vec2(1, 0);
				
		return vec2(atan(horizontaal.x, horizontaal.y), latitude);
	};

	for(size_t i=0; i<_punten->grootte(); i++)
		adresLengteBreedte[i] = berekenLengteBreedte(i);

	
	//Even wat dingen op een rijtje zetten om te gaan sorteren
	std::vector<vec3> punten = _punten->gegevens3(); //Werkt wat makkelijker

	std::vector<size_t> sorteerDit;
	sorteerDit.resize(punten.size());

	for(size_t i=0; i<sorteerDit.size(); i++)
		sorteerDit[i] = i;

	//Dan sorteren op de volgende wijze (in aflopende prioriteit): binnen-poolcirkels: { lengtegraden, breedtegraden }, boven/onder poolcirkels: {y, x, z}
	//Wat is een poolcirkel? Laten we zeggen... breedteG < 0.1 || breedteG > 0.9

	const float 	poolBreedte = 0.1, 
					poolA 		= poolBreedte, 
					poolB 		= 1.0 - poolBreedte;

	auto sortLambda = [&](const size_t & l, const size_t & r) -> bool
	{
		//Dit moet een < relatie worden uiteindelijk.
		const vec2 	& L = adresLengteBreedte[l],
					& R = adresLengteBreedte[r];

		if(L.y < poolA || L.y > poolB)
		{
			//Ok links is op de polen
			
			if(R.y >= poolA && R.y <= poolB)
				return false; //R is niet polair dus  !(L < R)
			else
			{
				//Allebei op de polen, maar zijn het dezelfde polen?

				if		(L.y < poolA && R.y > poolB)	return true;
				else if (L.y > poolB && R.y < poolA)	return false;
				
				//Ok, op dezelfde pool
				const vec3	& lPunt = punten[l],
							& rPunt = punten[r];

				if		(lPunt.x < rPunt.x)	return true;
				else if	(lPunt.x > rPunt.x) return false;
				
				return lPunt.z < rPunt.z;
			}
		}
		else
		{
			//L is iig niet op een pool

			if(R.y < poolA && R.y > poolB)
				return true; //Want R is op een pool en die komt dus zoiezo later

			//Hier zijn betekent dat zowel L als R op de brede band liggen.
			
			if(L.x < R.x)		return true;
			else if(L.x > R.x)	return false;
			else				return L.y < R.y;
		}
	};

	std::sort(sorteerDit.begin(), sorteerDit.end(), sortLambda);

	//Nu is alles dan gesorteerd, dus das leuk.
	//Alleen moeten we dat nog even aan de GPU duidelijk maken natuurlijk.

	_tex = new wrgvOnderOpslag<float>(2, _reeks, 1);

	std::map<size_t, size_t> oudNaarNieuw;

	for(size_t i=0; i<sorteerDit.size(); i++)
	{
		size_t verplaatsMe = sorteerDit[i];

		_punten->ggvPuntZetten(i, punten[verplaatsMe]);
		_tex->ggvPuntErbij(adresLengteBreedte[verplaatsMe]);

		oudNaarNieuw[verplaatsMe] = i;
	}

	_punten->spoel();
	_tex->spoel();

	//Verder moeten we natuurlijk niet vergeten de _drieHk op de hoogte te brengen!
	for(size_t i=0; i<_drieHk.size(); i++)
		_drieHk[i] = oudNaarNieuw[_drieHk[i]];
	
}