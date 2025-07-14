#include "plylezer.h"
#include <filesystem>
#include <iostream>

plyLezer::plyLezer()
{
	_reeks = new wrgvOpslag();
}

bool plyLezer::openPlyBestand(const std::filesystem::path & plyBestand)
{
	if(!std::filesystem::exists(plyBestand))
		return false;

	std::ifstream bestand = new std::ifstream(plyBestand);

	if(bestand.fail())
		return false;

	std::string regel;

	if(!std::getline(bestand, regel) || regel != "ply")
		return false;

	const std::string			formatS 		= "format",
								commentS 		= "comment",
								elementS		= "element vertex",
								propertyS		= "property float",
								endHeaderS		= "end_header";

	bool 						hoofdKlaar		= false;
	int							puntenGetal		= -1;
	std::vector<std::string>	eigenschappen;

	auto isDit = [&regel](const std::string & isHetDit) { return regel.size() >= isHetDit.size() && regel.substr(0, isHetDit.size()) == isHetDit; };

	for(;std::getline(bestand, regel) && hoofdKlaar;)
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
			std::string aantalPuntenS = regel.substr(elementS.size() + 2);

			if(!std::stoi(aantalPuntenS, &puntenGetal))
				throw std::runtime_error(regel);

			std::cout << "Aantal elementen: " << puntenGetal << std::endl;
		}
		else if(isDit(propertyS))
		{
			std::string eigenschap = regel.substr(propertyS.size() + 2);
			eigenschappen.push_back(eigenschap);
			std::cout << "Eigenschap: " << eigenschap << std::endl;
		}
		else if(isDit(endHeaderS))
		{
			hoofdKlaar = true;
			std::cout << "Hoofd ingelezen" << std::endl;
		}
		else
		{
			throw std::runtime_error(regel);
		}

	return true;
}