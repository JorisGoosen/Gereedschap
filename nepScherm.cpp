#include "nepScherm.h"
#include <iostream>

nepScherm::nepScherm(weergaveScherm * scherm, glm::uvec2 grootte, bool alsTextuur, bool metDiepteTesten)
:   _scherm(scherm), 
	_grootte(grootte),
	_textuurBasis(alsTextuur),
	_metDiepte(metDiepteTesten)
{
	if(!_textuurBasis)
		throw std::runtime_error("nepScherm zonder textuur wordt hier alleen nog maar ondersteund door een eigen textuur te bouwen...");

	gedeeldeBouwer();
}

nepScherm::nepScherm(weergaveScherm * scherm, const std::string & dezeTextuur, bool metDiepteTesten)
:   _scherm(scherm),
	_grootte(scherm->textuurGrootte(dezeTextuur)),
	_textuurBasis(true),
	_metDiepte(metDiepteTesten)
{
	//we tekenen in een bestaande textuur (die moet wel renderbaar zijn, en dat zijn ze)
	_textuurId = scherm->textuurId(dezeTextuur);
}

void nepScherm::gedeeldeBouwer(int dezeTextuur)
{
	static int nepSchermTextuurTeller = 0;

	(void)dezeTextuur;

	_textuurId = _scherm->maakTextuur("nepSchermTextuur#" + std::to_string(nepSchermTextuurTeller++), _grootte.x, _grootte.y, false, false, false);
}


void nepScherm::bereidWeergevenVoor(const std::string & verwerker, bool wisScherm)
{
	//zorg dat het scherm in onze textuur tekent in plaats van op het venster
	_scherm->zetWeergaveDoel(_textuurId, _grootte);

	_scherm->_bereidWeergevenVoor(verwerker, wisScherm, _grootte.x, _grootte.y);
}


void nepScherm::rondWeergevenAf()
{
	//rondWeergevenAf ziet dat er naar een doel-textuur werd getekend en zal daarom niet presenteren
	_scherm->rondWeergevenAf();
}