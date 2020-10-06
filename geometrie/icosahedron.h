#include "../RenderSubBuffer.h"

#define ICOSAHEDRON_PUNTEN  12
#define ICOSAHEDRON_LIJNEN  30
#define ICOSAHEDRON_VLAKKEN 20

struct Lijn
{
	unsigned int a, b;

	bool		operator==(const Lijn & ander) const { return (a == ander.a && b == ander.b) || (a == ander.b && b == ander.a); }
	bool 		heeftPunt(int p) 																								const;
	bool 		vormtDriehoek(				const Lijn & deEen, const Lijn & deAnder) 											const;
	glm::ivec3	geefDriehoek(				const Lijn & deEen, const Lijn & deAnder) 											const;
	glm::ivec3 	geefGeorienteerdeDriehoek(	const Lijn & deEen, const Lijn & deAnder, const RenderSubBuffer<float> * punten)	const;
};

class Icosahedron
{
public:
	Icosahedron();

	///Teken jezelf met driehoeken
	void tekenJezelf() 			const; 

	///Teken jezelf met patches, tbv vlakverdeling
	void tekenJezelfPatchy() 	const;
	
private:
	void genereer(bool PrettyIcoInsteadOfJensens = true);

	RenderBuffers			* _icoArray  = nullptr;
	RenderSubBuffer<float>			* _icoPunten = nullptr;
	std::vector<glm::uint32>	  	  _icoDriehk;
};