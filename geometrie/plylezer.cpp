#include "plylezer.h"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>

plyLezer::plyLezer()
{
	_reeks 	= new wrgvOpslag();
	_xyz	= new wrgvOnderOpslag<float>(3, _reeks, 0);
	_nxyz	= new wrgvOnderOpslag<float>(3, _reeks, 1);
}

bool plyLezer::openPlyBestand(const std::filesystem::path & plyBestand)
{
	if(!std::filesystem::exists(plyBestand))
	{
		std::cerr << plyBestand.generic_string() << " niet gevonden in bestandssysteem!" << std::endl;
		return false;
	}

	std::ifstream bestand(plyBestand, std::ios::binary);

	if(bestand.fail())
	{
		std::cerr << "ifstream openen gefaald!" << std::endl;
		return false;
	}

	std::string regel;

	if(!std::getline(bestand, regel, char(0x0a)) || regel != "ply")
	{
		std::cerr << "Geen ply magic string: '" << regel << "'" << std::endl;
		return false;
	}

	const std::string			formatS 		= "format",
								commentS 		= "comment",
								elementS		= "element vertex",
								propertyS		= "property float",
								endHeaderS		= "end_header";

	bool 						hoofdKlaar		= false;
	int							puntenGetal		= -1;
	std::vector<std::string>	eigenschappen;

	auto isDit = [&regel](const std::string & isHetDit) { return regel.size() >= isHetDit.size() && regel.substr(0, isHetDit.size()) == isHetDit; };

	for(;!hoofdKlaar && std::getline(bestand, regel, char(0x0a));)
	{
		std::cout << regel << std::endl;
		if(isDit(formatS))
		{
			if(regel != "format binary_little_endian 1.0")
				throw std::runtime_error(regel);
		}
		else if(isDit(commentS))
		{
			// doe niets
		}
		else if(isDit(elementS))
		{
			//element vertex 209317
			std::string aantalPuntenS = regel.substr(elementS.size() + 1);

			puntenGetal = std::stoi(aantalPuntenS);

			std::cout << "Aantal elementen: " << puntenGetal << std::endl;
		}
		else if(isDit(propertyS))
		{
			std::string eigenschap = regel.substr(propertyS.size() + 1);
			eigenschappen.push_back(eigenschap);
			std::cout << "Eigenschap #" << eigenschappen.size() << ": " << eigenschap << std::endl;
		}
		else if(isDit(endHeaderS))
		{
			hoofdKlaar = true;
			std::cout << "Hoofd klaar" << std::endl;
		}
		else
		{
			throw std::runtime_error(regel);
		}
	}

	// Nu hebben we het hoofd ingelezen en willen we dus the kleine kant eerst float een voor een gaan inlezen en in de juist wrgOnderOpslag onder brengen

	size_t eigenschapTeller = 0;

	std::map<std::string, float> opgehaald;

	auto verwerkOpgehaaldeFloats = [&]()
	{
		_xyz	->ggvPuntErbij(glm::vec3(opgehaald.at("x"), 		opgehaald.at("y"), 		opgehaald.at("z")));
		_nxyz	->ggvPuntErbij(glm::vec3(opgehaald.at("nx"), 		opgehaald.at("ny"), 	opgehaald.at("nz")));

		opgehaald.clear();

		eigenschapTeller = 0;
	};

	int floatsGeteld=0;
	float f;

	do
	{
		bestand.read(reinterpret_cast<char*>(&f), sizeof(float));
		floatsGeteld++;
		
		//std::cout << f << std::endl;

		opgehaald[eigenschappen[eigenschapTeller++]] = f;

		if(eigenschapTeller == eigenschappen.size())
			verwerkOpgehaaldeFloats();	
	}
	while(bestand.good());

	//verwerkOpgehaaldeFloats();

	_xyz	->spoel();
	_nxyz	->spoel();

	std::cout << "_xyz->grootte(): " << _xyz->grootte() << " puntenGetal: " << puntenGetal << " floatsGeteld: " << floatsGeteld << std::endl;
	assert(_xyz->grootte() == puntenGetal);

	return true;
}

void plyLezer::tekenJezelf() const
{
	_reeks->bindPuntReeks();
	glDrawArrays(GL_POINTS, _reeks->reeksBufferVoorPlek(0), _xyz->grootte());
	glErrorToConsole("plyLezer::tekenJezelf(): ");
}