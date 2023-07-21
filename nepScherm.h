#include "weergaveScherm.h"

class nepScherm
{
public:
    nepScherm(weergaveScherm * scherm, glm::uvec2 grootte, bool alsTextuur = true, bool metDiepteTesten = false);
    ~nepScherm() { glDeleteFramebuffers(1, &_nepSchermId); }

    void bereidWeergevenVoor(const std::string & verwerker = "", bool wisScherm = true);
    void rondWeergevenAf();

    weergaveScherm * scherm() { return _scherm; }

protected:
    weergaveScherm  *   _scherm;
    glm::uvec2          _grootte;
    bool                _textuurBasis,
                        _metDiepte;
    GLuint              _nepSchermId,
                        _textuurId,
                        _diepteId;


};