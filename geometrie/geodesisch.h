#include "icosahedron.h"
#include <stdexcept>
#include <map>
#include <set>
#include <iostream>
class geodesisch : public icosahedron
{
public:	
	geodesisch(size_t onderverdelingen = 1);

protected:
	void verdeelEnHeers();
	void ordenPunten();

private:
	size_t 						_onderverdelingen;
	wrgvOnderOpslag<float>	* 	_tex = nullptr;

};