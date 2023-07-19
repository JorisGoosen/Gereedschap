#include "weergaveScherm.h"

class nepScherm
{
public:
    nepScherm(weergaveScherm * scherm, glm::uvec2 grootte, bool alsTextuur = false, bool metDiepteTesten = true);
    ~nepScherm() { glDeleteFramebuffers(1, &_nepSchermId); }

protected:
    weergaveScherm  *   _scherm;
    glm::uvec2          _grootte;
    bool                _textuurBasis,
                        _metDiepte;
    GLuint              _nepSchermId,
                        _textuurId,
                        _diepteId,
                        _weergaveOpslagId;


};