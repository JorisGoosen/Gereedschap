#include "icosahedron.h"

class Geodesisch : public Icosahedron
{
public:
	Geodesisch(size_t onderverdelingen = 1);

protected:
	void verdeelEnHeers();

private:
	size_t _onderverdelingen;
};