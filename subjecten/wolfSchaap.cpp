#include "wolfSchaap.h"
#include "../weergaveScherm.h"
#include "../geometrie/vierkantRooster.h"
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


Dieren::Dieren(weergaveScherm * scherm, int aantalWolven, int aantalSchapen, float wereldGrootte)
: _wereldGrootte(wereldGrootte), _aantalWolven(aantalWolven), _aantalSchapen(aantalSchapen)
{
	_scherm = scherm;
	hackyWereldGrootte = _wereldGrootte;

	std::vector<Dier>			wolven,
								schapen;
	std::vector<PlaatsKleur>	wolfPos,
								schaapPos;

	wolven		.resize(aantalWolven);
	schapen		.resize(aantalSchapen);
	wolfPos		.resize(aantalWolven);
	schaapPos	.resize(aantalSchapen);

	_wolvenE .push_back(new vrwrkrOpslagDing<Dier>(			wolven,  	0));
	_wolvenE .push_back(new vrwrkrOpslagDing<Dier>(			wolven,  	1));
	_wolvenP .push_back(new vrwrkrOpslagDing<PlaatsKleur>(	wolfPos, 	2));
	_wolvenP .push_back(new vrwrkrOpslagDing<PlaatsKleur>(	wolfPos, 	3));
	_schapenE.push_back(new vrwrkrOpslagDing<Dier>(			schapen, 	4));
	_schapenE.push_back(new vrwrkrOpslagDing<Dier>(			schapen, 	5));
	_schapenP.push_back(new vrwrkrOpslagDing<PlaatsKleur>(	schaapPos, 	6));
	_schapenP.push_back(new vrwrkrOpslagDing<PlaatsKleur>(	schaapPos, 	7));

	//het vierkantje (twee driehoeken) dat op elke plek wordt neergezet
	const float grootte = 0.03f;
	const float kaders[12] = {
		-grootte, -grootte,  grootte, -grootte,  grootte,  grootte,
		-grootte, -grootte,  grootte,  grootte, -grootte,  grootte,
	};

	WGPUBufferDescriptor beschrijving = WGPU_BUFFER_DESCRIPTOR_INIT;
	beschrijving.usage = WGPUBufferUsage_Vertex | WGPUBufferUsage_CopyDst;
	beschrijving.size  = sizeof(kaders);
	_vierkantje = wgpuDeviceCreateBuffer(gedeeldApparaat(), &beschrijving);
	wgpuQueueWriteBuffer(gedeeldeRij(), _vierkantje, 0, kaders, sizeof(kaders));
}

Dieren::~Dieren()
{
	for(auto * ding : _wolvenE)		delete ding;
	for(auto * ding : _wolvenP)		delete ding;
	for(auto * ding : _schapenE)	delete ding;
	for(auto * ding : _schapenP)	delete ding;

	if(_vierkantje)		wgpuBufferRelease(_vierkantje);
}

void Dieren::teken(bool wolven)
{
	auto	& 	plekken 	= wolven ? _wolvenP : _schapenP;
	int 		aantal 		= wolven ? _aantalWolven : _aantalSchapen;

	if(!_scherm)
		return;

	_scherm->tekenKadertjes(_vierkantje, 6, plekken[_pingPong]->opslag(), aantal);
}

void Dieren::pong()
{
	_pingPong = 1 - _pingPong;
}

void Dieren::beweeg(bool wolven)
{
	auto	& 	plaatsE 	= wolven ? _wolvenE : _schapenE;
	auto	& 	plaatsP 	= wolven ? _wolvenP : _schapenP;
	int 		aantal 		= wolven ? _aantalWolven : _aantalSchapen;

	std::function<void()> voorbereiding = [&]()
	{
		_scherm->verbindRekenBuffer(0, plaatsP[  _pingPong]->opslag());	// wijZijnP
		_scherm->verbindRekenBuffer(1, plaatsP[1-_pingPong]->opslag());	// wijWordenP
		_scherm->verbindRekenBuffer(2, plaatsE[  _pingPong]->opslag());	// wijZijnE
		_scherm->verbindRekenBuffer(3, plaatsE[1-_pingPong]->opslag());	// wijWordenE
	};

	if(_scherm)
	{
		_scherm->doeRekenVerwerker("beweeg", glm::uvec3(aantal, 1, 1), voorbereiding);
		_scherm->ontkoppelRekenBuffers();
	}
}