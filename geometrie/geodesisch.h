#include "icosahedron.h"
#include <stdexcept>
#include <map>
#include <set>
#include <iostream>
class Geodesisch : public icosahedron
{
public:
	typedef std::map<glm::uint32, std::set<glm::uint32>> buurt;
	
	Geodesisch(size_t onderverdelingen = 1);

protected:
	void verdeelEnHeers();
	void maakLijstBuren();
	

private:
	wrgvOnderOpslag<glm::uint32>	* 	_wrgvBuren 			= nullptr;
	size_t 								_onderverdelingen;
	buurt								_buren;
};