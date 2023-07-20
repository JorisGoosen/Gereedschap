#include "wolfSchaap.h"
#include "../weergaveScherm.h"
#include <random>
#include <numbers>

std::random_device						zaaier;
std::normal_distribution<float> 		willekeur(0.0, 1.0);
std::uniform_real_distribution<float>	nietNormaal(0.0, 1.0);

Dier::Dier()
{
	heteroKracht 	= willekeur(zaaier);
	homoKracht 		= willekeur(zaaier);
	honger			= 0.0;
	levend			= 1;
}

float hackyWereldGrootte	= 2;
bool hackySchapenNu			= false;

PlaatsKleur::PlaatsKleur()
{
	float 	hoek		= nietNormaal(zaaier) * std::numbers::pi * 2.0,
			afstand		= nietNormaal(zaaier) * hackyWereldGrootte;

	posX			= sin(hoek) * afstand;
	posY			= cos(hoek) * afstand;
	lichtheid		= (hackySchapenNu ? 1 	: 0.5)	* nietNormaal(zaaier);
	roodheid		= (hackySchapenNu ? 0.2 : 1.0) 	* nietNormaal(zaaier);

}


Dieren::Dieren(int aantalWolven, int aantalSchapen, float wereldGrootte)
: _wereldGrootte(wereldGrootte), _aantalWolven(aantalWolven), _aantalSchapen(aantalSchapen)
{
	hackyWereldGrootte = _wereldGrootte;

	std::vector<Dier>			wolven,
								schapen;
	std::vector<PlaatsKleur>	wolfPos,
								schaapPos;

	wolven.resize(aantalWolven);
	schapen.resize(aantalSchapen);
	wolfPos.resize(aantalWolven);
	schaapPos.resize(aantalSchapen);

	_wolvenE .push_back(new vrwrkrOpslagDing<Dier>(			wolven,  	0));
	_wolvenE .push_back(new vrwrkrOpslagDing<Dier>(			wolven,  	1));
	_wolvenP .push_back(new vrwrkrOpslagDing<PlaatsKleur>(	wolfPos, 	2));
	_wolvenP .push_back(new vrwrkrOpslagDing<PlaatsKleur>(	wolfPos, 	3));
	_schapenE.push_back(new vrwrkrOpslagDing<Dier>(			schapen, 	4));
	_schapenE.push_back(new vrwrkrOpslagDing<Dier>(			schapen, 	5));
	_schapenP.push_back(new vrwrkrOpslagDing<PlaatsKleur>(	schaapPos, 	6));
	_schapenP.push_back(new vrwrkrOpslagDing<PlaatsKleur>(	schaapPos, 	7));

	_wolvenP [0]->maakReeksOpslag();
	_wolvenP [1]->maakReeksOpslag();
	_schapenP[0]->maakReeksOpslag();
	_schapenP[1]->maakReeksOpslag();

	glErrorToConsole("Dieren::Dieren(wolven=" + std::to_string(aantalWolven) + ", schapen=" + std::to_string(aantalSchapen) + ", wereldGrootte=" + std::to_string(wereldGrootte) + "): ");
}

void Dieren::teken(bool wolven)
{
	if(wolven)	_wolvenP [0]->bindPuntReeks();
	else		_schapenP[0]->bindPuntReeks();

	glDrawArrays( GL_POINTS, 0, wolven ? _aantalWolven : _aantalSchapen);

	glErrorToConsole(std::string("Dieren::teken(") + (wolven ? "wolven":"schapen") + "): ");
}

void Dieren::pong()
{
	_pingPong = 1 - _pingPong;
}

void Dieren::beweeg(weergaveScherm * scherm, bool wolven)
{

	std::function<void()> schaapVoorbereiding = [&]()
	{

		_schapenE[  _pingPong]->zetKnooppunt(glGetUniformBlockIndex(scherm->huidigProgramma(), "wijZijnE"));
		_schapenE[1-_pingPong]->zetKnooppunt(glGetUniformBlockIndex(scherm->huidigProgramma(), "wijWorden"));
		_schapenP[  _pingPong]->zetKnooppunt(glGetUniformBlockIndex(scherm->huidigProgramma(), "wijZijnP"));
		_schapenP[1-_pingPong]->zetKnooppunt(glGetUniformBlockIndex(scherm->huidigProgramma(), "wijWorden"));

		//glUniform1i(glGetUniformLocation(scherm->huidigProgramma(), "pingPong"), _pingPong);
	};


//	scherm->doeRekenVerwerker("beweeg", glm::uvec3(_aantalSchapen, 1, 1), schaapVoorbereiding);

	glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT);
}

/*
void Dieren::bindDingenRekenVerwerker(weergaveScherm * scherm, const std::string & verwerker, bool pong)
{
	if(pong)
		_pingPong = !_pingPong;

	std::function<void(int)> DierBinder = [&](int prog)
	{
		glUniform1i(glGetUniformLocation(scherm->huidigProgramma(), "pingPong"), _pingPong);
		glUniform1i(glGetUniformLocation(scherm->huidigProgramma(), "pingPong"), _pingPong);
	};

	scherm->doeRekenVerwerker(verwerker, {_aantalWolven, 1, 1}, wolfBinder);
}*/

