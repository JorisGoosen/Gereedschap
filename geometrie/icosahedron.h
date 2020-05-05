#include "../RenderSubBuffer.h"

#define ICOSAHEDRON_PUNTEN  12
#define ICOSAHEDRON_LIJNEN  30
#define ICOSAHEDRON_VLAKKEN 20

struct Lijn
{
	unsigned int a, b;
};

class Icosahedron
{
public:
	Icosahedron() { genereer(); }


private:
	void genereer(bool PrettyIcoInsteadOfJensens = true);

	glm::vec4 	_punten[ICOSAHEDRON_PUNTEN];
	Lijn		_lijnen[ICOSAHEDRON_LIJNEN];

};