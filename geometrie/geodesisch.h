#include "icosahedron.h"
#include <stdexcept>
#include <map>
#include <set>
#include <iostream>
class Geodesisch : public icosahedron
{
public:
	typedef std::vector<std::set<glm::uint32>> buurt;
	
	Geodesisch(size_t onderverdelingen = 1);

protected:
	void verdeelEnHeers();
	void maakLijstBuren();
	void burenAlsEigenschapWijzers();
	

private:
	std::vector<glm::uint32> 			_eigenschappen;
	size_t 								_onderverdelingen;
	buurt								_buren;
};