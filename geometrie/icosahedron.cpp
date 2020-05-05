#include "icosahedron.h"

using namespace glm;

void Icosahedron::genereer(bool PrettyIcoInsteadOfJensens)
{
	float t = (1.0 + sqrtf(5.0)) / 2.0;

	_punten[0]  = vec4(	  -t,	 1.0f,	 0.0f,	1.0f);
	_punten[1]  = vec4(	   t,	 1.0f,	 0.0f,	1.0f);
	_punten[2]  = vec4(	  -t,	-1.0f,	 0.0f,	1.0f);
	_punten[3]  = vec4(	   t,	-1.0f,	 0.0f,	1.0f);

	_punten[4]  = vec4(	0.0f,	   -t,	 1.0f,	1.0f);
	_punten[5]  = vec4(	0.0f,	    t,	 1.0f,	1.0f);
	_punten[6]  = vec4(	0.0f,	   -t,	-1.0f,	1.0f);
	_punten[7]  = vec4(	0.0f,	    t,	-1.0f,	1.0f);

	_punten[8]  = vec4(	 1.0f,	 0.0f,	   -t,	1.0f);
	_punten[9]  = vec4(	 1.0f,	 0.0f,	    t,	1.0f);
	_punten[10] = vec4(	-1.0f,	 0.0f,	   -t,	1.0f);
	_punten[11] = vec4(	-1.0f,	 0.0f,	    t,	1.0f);

	if(PrettyIcoInsteadOfJensens) //Zet dit op false om een Jensen's icosahedron te krijgen (is wel lelijk though)
	{
		_lijnen[29].a= 0;	_lijnen[29].b = 2;
		_lijnen[28].a= 1;	_lijnen[28].b = 3;
		_lijnen[23].a= 8;	_lijnen[23].b = 10;
		_lijnen[18].a= 9;	_lijnen[18].b = 11;
		_lijnen[9].a = 4;	_lijnen[9].b  = 6;
		_lijnen[0].a = 5;	_lijnen[0].b  = 7;
	}else{
		_lijnen[0].a  = 0;	_lijnen[0].b  = 1;
		_lijnen[9].a  = 2;	_lijnen[9].b  = 3;
		_lijnen[18].a = 4;	_lijnen[18].b = 5;
		_lijnen[23].a = 6;	_lijnen[23].b = 7;
		_lijnen[28].a = 8;	_lijnen[28].b = 9;
		_lijnen[29].a = 10;	_lijnen[29].b = 11;
	}

	_lijnen[1].a  = 0;	_lijnen[1].b  = 5;
	_lijnen[2].a  = 0;	_lijnen[2].b  = 7;
	_lijnen[3].a  = 0;	_lijnen[3].b  = 10;
	_lijnen[4].a  = 0;	_lijnen[4].b  = 11;
	_lijnen[5].a  = 1;	_lijnen[5].b  = 5;
	_lijnen[6].a  = 1;	_lijnen[6].b  = 7;
	_lijnen[7].a  = 1;	_lijnen[7].b  = 8;
	_lijnen[8].a  = 1;	_lijnen[8].b  = 9;
	_lijnen[10].a = 2;	_lijnen[10].b = 4;
	_lijnen[11].a = 2;	_lijnen[11].b = 6;
	_lijnen[12].a = 2;	_lijnen[12].b = 10;
	_lijnen[13].a = 2;	_lijnen[13].b = 11;
	_lijnen[14].a = 3;	_lijnen[14].b = 4;
	_lijnen[15].a = 3;	_lijnen[15].b = 6;
	_lijnen[16].a = 3;	_lijnen[16].b = 8;
	_lijnen[17].a = 3;	_lijnen[17].b = 9;
	_lijnen[19].a = 4;	_lijnen[19].b = 9;
	_lijnen[20].a = 4;	_lijnen[20].b = 11;
	_lijnen[21].a = 5;	_lijnen[21].b = 9;
	_lijnen[22].a = 5;	_lijnen[22].b = 11;
	_lijnen[24].a = 6;	_lijnen[24].b = 8;
	_lijnen[25].a = 6;	_lijnen[25].b = 10;
	_lijnen[26].a = 7;	_lijnen[26].b = 8;
	_lijnen[27].a = 7;	_lijnen[27].b = 10;
}
