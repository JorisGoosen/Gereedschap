#include "wolfSchaap.h"
#include "../weergaveScherm.h"
#include <random>
#include <numbers>

std::random_device						zaaier;
std::normal_distribution<float> 		willekeur(-1.0, 1.0);
std::uniform_real_distribution<float>	nietNormaal(0.0, 1.0);

Dier::Dier()
{
	heteroKracht 	= willekeur(zaaier);
	homoKracht 		= willekeur(zaaier);
	honger			= 0.0;
	levend			= 1;
}

float hackyWereldGrootte	= 3;
bool hackySchapenNu			= false;

PlaatsKleur::PlaatsKleur()
{
	float 	hoek		= nietNormaal(zaaier) * std::numbers::pi * 2.0,
			afstand		= nietNormaal(zaaier) * hackyWereldGrootte;

	posX			= sin(hoek) * afstand;
	posY			= cos(hoek) * afstand;

	glm::vec2 s(posX, posY);
	s = glm::normalize(s);
	lichtheid		= -s.x;//willekeur(zaaier);//(hackySchapenNu ? 1 	: 0.5)	* nietNormaal(zaaier);
	roodheid		= -s.y;//willekeur(zaaier);//(hackySchapenNu ? 0.2 : 1.0) 	* nietNormaal(zaaier);

}


Dieren::Dieren(weergaveScherm * scherm, int aantalWolven, int aantalSchapen, float wereldGrootte)
: _wereldGrootte(wereldGrootte), _aantalWolven(aantalWolven), _aantalSchapen(aantalSchapen)
{
	hackyWereldGrootte = _wereldGrootte;

	//_vierkant 	= new vierkantRooster(1, 1);
	_scherm 	= scherm;

	std::vector<Dier>			wolven,
								schapen;
	std::vector<PlaatsKleur>	wolfPos,
								schaapPos;

	wolven		.resize(aantalWolven);
	schapen		.resize(aantalSchapen);
	wolfPos		.resize(aantalWolven);
	schaapPos	.resize(aantalSchapen);

	for(auto & wp : wolfPos)
	{
		wp.posX *= 6.0;
		wp.posY *= 4.0;
	}

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

	_wolvenE [0]->maakReeksOpslag();
	_wolvenE [1]->maakReeksOpslag();
	_schapenE[0]->maakReeksOpslag();
	_schapenE[1]->maakReeksOpslag();

	glErrorToConsole("Dieren::Dieren(wolven=" + std::to_string(aantalWolven) + ", schapen=" + std::to_string(aantalSchapen) + ", wereldGrootte=" + std::to_string(wereldGrootte) + "): ");
}

void Dieren::teken(bool wolven)
{
	if(wolven)	_wolvenP [0]->bindPuntReeks();
	else		_schapenP[0]->bindPuntReeks();

	glUniform1ui(	glGetUniformLocation(_scherm->huidigProgramma(), "jeBentWolf"), 		wolven);

	glDrawArrays( GL_POINTS, 0, wolven ? _aantalWolven : _aantalSchapen);

	glErrorToConsole(std::string("Dieren::teken(") + (wolven ? "wolven":"schapen") + "): ");
}

void Dieren::pong()
{
	_pingPong = 1 - _pingPong;
}

void Dieren::beweeg(bool wolven)
{
	_scherm->laadOmgeving();

	glFlush();
	glMemoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT);

	glErrorToConsole("Dieren::beweeg");
	std::function<void()> schaapVoorbereiding = [&]()
	{
		auto & wijE =  wolven ? _wolvenE : _schapenE;
		auto & wijP =  wolven ? _wolvenP : _schapenP;
		auto & zijP = !wolven ? _wolvenP : _schapenP;

		wijE[  _pingPong]->zetKnooppunt(0);
		wijE[1-_pingPong]->zetKnooppunt(1);
		wijP[  _pingPong]->zetKnooppunt(2);
		wijP[1-_pingPong]->zetKnooppunt(3);
		zijP[  _pingPong]->zetKnooppunt(4);

		glErrorToConsole("Dieren:::schaapVoorbereiding knooppunten");

		glUniform1ui(	glGetUniformLocation(_scherm->huidigProgramma(), "onzeLengte"), 		wijP[0]->grootte());

		glErrorToConsole("Dieren:::schaapVoorbereiding uniform onzeLengte");
		glUniform1ui(	glGetUniformLocation(_scherm->huidigProgramma(), "anderLengte"), 		zijP[0]->grootte());

		glErrorToConsole("Dieren:::schaapVoorbereiding uniform anderLengte");
		glUniform1ui(	glGetUniformLocation(_scherm->huidigProgramma(), "jeBentWolf"), 		wolven);

		glErrorToConsole("Dieren:::schaapVoorbereiding uniform jeBentWolf");
		//glUniform1i(glGetUniformLocation(scherm->huidigProgramma(), "pingPong"), _pingPong);
	};


	_scherm->doeRekenVerwerker("beweeg", glm::uvec3(_aantalSchapen, 1, 1), schaapVoorbereiding);

	glFlush();
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

