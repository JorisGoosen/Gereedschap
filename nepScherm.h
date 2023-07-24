#include "weergaveScherm.h"

class nepScherm
{
public:
    nepScherm(weergaveScherm * scherm, glm::uvec2 grootte, bool alsTextuur = true, bool metDiepteTesten = false);
    nepScherm(weergaveScherm * scherm, const std::string & dezeTextuur, bool metDiepteTesten = false);

    ~nepScherm() { glDeleteFramebuffers(1, &_nepSchermId); }

    void bereidWeergevenVoor(const std::string & verwerker = "", bool wisScherm = true);
    void rondWeergevenAf();
    void gedeeldeBouwer(int dezeTextuur = -1);

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