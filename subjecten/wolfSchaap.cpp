#include "wolfSchaap.h"
#include "weergaveScherm.h"
#include <random>

std::random_device				zaaier;
std::normal_distribution<float> willekeur(0.0, 1.0);

dier::dier()
{
	heteroKracht 	= willekeur(zaaier);
	homoKracht 		= willekeur(zaaier);
	honger			= 0.0;
}

dieren::dieren(int aantalWolven, int aantalSchapen, float wereldGrootte)
: _wereldGrootte(wereldGrootte), _aantalWolven(aantalWolven), _aantalSchapen(aantalSchapen)
{
	std::vector<dier>	wolven,
						schapen;

	wolven.resize(aantalWolven);
	schapen.resize(aantalSchapen);

	_wolven ->push_back(new vrwrkrOpslagDing<dier>(wolven,  0));
	_wolven ->push_back(new vrwrkrOpslagDing<dier>(wolven,  1));
	_schapen->push_back(new vrwrkrOpslagDing<dier>(schapen, 2));
	_schapen->push_back(new vrwrkrOpslagDing<dier>(schapen, 3));

	glErrorToConsole("dieren::dieren(wolven=" + std::to_string(aantalWolven) + ", schapen=" + std::to_string(aantalSchapen) + ", wereldGrootte=" + std::to_string(wereldGrootte) + "): ");
}

void dieren::bindDingen(weergaveScherm * scherm, const std::string & verwerker, bool pong)
{
	if(pong)
		_pingPong = !_pingPong;


	scherm->doeRekenVerwerker(verwerker, 
}