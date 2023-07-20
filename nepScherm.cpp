#include "nepScherm.h"

nepScherm::nepScherm(weergaveScherm * scherm, glm::uvec2 grootte, bool alsTextuur, bool metDiepteTesten)
:   _scherm(scherm), 
    _grootte(grootte),
    _textuurBasis(alsTextuur),
    _metDiepte(metDiepteTesten)
{
    glGenFramebuffers(1, &_nepSchermId);

    glBindFramebuffer(GL_FRAMEBUFFER, _nepSchermId);

    if(_textuurBasis)
    {
        static int nepSchermTextuurTeller = 0;
        _textuurId = _scherm->maakTextuur("nepSchermTextuur#" + std::to_string(nepSchermTextuurTeller++), _grootte.x, _grootte.y);
        glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, _textuurId, 0);

        if(_metDiepte)
        {
            _diepteId = _scherm->maakTextuur("nepSchermDiepteTextuur#" + std::to_string(nepSchermTextuurTeller-1), _grootte.x, _grootte.y, false, false, false, GL_DEPTH_COMPONENT16, nullptr, GL_2_BYTES, GL_2_BYTES);
            glFramebufferTexture2D(GL_DRAW_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, _diepteId, 0);
        }
    }
    else
    {
        throw std::runtime_error("luiwammes");
    }
}


void nepScherm::bereidWeergevenVoor(const std::string & verwerker, bool wisScherm)
{
	_scherm->laadOmgeving();

	glBindFramebuffer(GL_FRAMEBUFFER, _nepSchermId);
    
    glErrorToConsole("nepScherm::bereidWeergevenVoor weergave opslag binden: ");

	_scherm->_bereidWeergevenVoor(verwerker, wisScherm, _grootte.x, _grootte.y);
}


void nepScherm::rondWeergevenAf()
{
    glFlush();
    glErrorToConsole("nepScherm::rondWeergevenAf: ");
}