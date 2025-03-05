#include "../wrgvOnderOpslag.h"
#include <array>

#define ICOSAHEDRON_PUNTEN  12
#define ICOSAHEDRON_LIJNEN  30
#define ICOSAHEDRON_VLAKKEN 20

struct lijn
{
	unsigned int a, b;

	bool		operator==(const lijn & ander) const { return (a == ander.a && b == ander.b) || (a == ander.b && b == ander.a); }
	bool 		heeftPunt(unsigned int p) 																						const;
	bool 		vormtDriehoek(				const lijn & deEen, const lijn & deAnder) 											const;
	glm::ivec3	geefDriehoek(				const lijn & deEen, const lijn & deAnder) 											const;
	glm::ivec3 	geefGeorienteerdeDriehoek(	const lijn & deEen, const lijn & deAnder, const wrgvOnderOpslag<float> * punten)	const;
};

class icosahedron
{
public:
	icosahedron();

	///Teken jezelf met driehoeken
	virtual void tekenJezelf() 			const; 

	///Teken jezelf met patches, tbv vlakverdeling
	virtual void tekenJezelfPatchy() 	const;

	void bindPuntReeks() const { _reeks->bindPuntReeks();	 }
	
protected:
	void genereer();

	wrgvOpslag						* _reeks  = nullptr;
	wrgvOnderOpslag<float>			* _punten = nullptr;
	std::vector<glm::uint32>	  	  _drieHk;
};