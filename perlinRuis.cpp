#include "perlinRuis.h"
#include <random>

perlinRuis::perlinRuis() 
: _bemonsteraar(_willekeur())

{
	_perlin 		= this;

	maakGradienten();
	maakHerschikkingsTabel();
}

perlinRuis * perlinRuis::_perlin = nullptr;

void perlinRuis::zetKnooppunten(int gradientAdres, int herschikkingsAdres)
{
	_willekeurigeGradienten	-> zetKnooppunt(gradientAdres);
	_herschikkingsTabel		-> zetKnooppunt(herschikkingsAdres);
}
		

perlinRuis::~perlinRuis()
{
	delete _willekeurigeGradienten;
	delete _herschikkingsTabel;

	_willekeurigeGradienten		= nullptr;
	_herschikkingsTabel			= nullptr;
}

void perlinRuis::maakGradienten()
{
	if(_willekeurigeGradienten != nullptr) delete _willekeurigeGradienten;

	std::uniform_real_distribution<> greep(-1.0, 1.0);

	_gradienten.reserve(PERLIN_AANTAL_GRADIENTEN);

	for(size_t i=0; i<PERLIN_AANTAL_GRADIENTEN; i++)
	{
        glm::vec3 P;

		do { P = glm::vec3(greep(_bemonsteraar), greep(_bemonsteraar), greep(_bemonsteraar)); } while(glm::length(P) > 1.0 || glm::length(P) == 0.0f);

        _gradienten.push_back(glm::normalize(P));
	}

	_willekeurigeGradienten = new vrwrkrOpslagDing(_gradienten, 2);
}

void perlinRuis::maakHerschikkingsTabel()
{
	if(_herschikkingsTabel != nullptr) delete _herschikkingsTabel;

    
	_tabel.resize(PERLIN_AANTAL_GRADIENTEN * 2);

	for(int i=0; i<PERLIN_AANTAL_GRADIENTEN; i++)
		_tabel[i] = i;

	for(int i=0; i<PERLIN_AANTAL_GRADIENTEN; i++)
		std::swap(_tabel[i], _tabel[_bemonsteraar()%PERLIN_AANTAL_GRADIENTEN]);

	for(int i=0; i<PERLIN_AANTAL_GRADIENTEN; i++)
		_tabel[i + PERLIN_AANTAL_GRADIENTEN] = _tabel[i];
	
	_herschikkingsTabel = new vrwrkrOpslagDing<int>(_tabel, 3);
}


glm::vec3 perlinRuis::gradient(glm::ivec3 loc)
{
    int ijk[3];
    for(int i=0; i<3; i++)
		ijk[i] = (PERLIN_AANTAL_GRADIENTEN + ((int)loc[i])) % PERLIN_AANTAL_GRADIENTEN;

    return _gradienten[
		_tabel[
			_tabel[
				_tabel[
					ijk[0]	] +
				ijk[1]		] +
			ijk[2]		
		]
	];
}


float perlinRuis::geefIniqoQuilesRuis(glm::vec3 x)
{
	x *= 16.0f;
    // grid
    glm::vec3 p = glm::floor(x), w = glm::fract(x);

    // quintic interpolant
    glm::vec3 u = w * w * w * (w * (w * glm::vec3(6.0f - 15.0f))+glm::vec3(10.0f));

    
    // gradients
    glm::vec3 ga = gradient(p + glm::vec3(0.0f, 0.0f, 0.0f));
    glm::vec3 gb = gradient(p + glm::vec3(1.0f, 0.0f, 0.0f));
    glm::vec3 gc = gradient(p + glm::vec3(0.0f, 1.0f, 0.0f));
    glm::vec3 gd = gradient(p + glm::vec3(1.0f, 1.0f, 0.0f));
    glm::vec3 ge = gradient(p + glm::vec3(0.0f, 0.0f, 1.0f));
    glm::vec3 gf = gradient(p + glm::vec3(1.0f, 0.0f, 1.0f));
    glm::vec3 gg = gradient(p + glm::vec3(0.0f, 1.0f, 1.0f));
    glm::vec3 gh = gradient(p + glm::vec3(1.0f, 1.0f, 1.0f));
    
    // projections
    float va = glm::dot(ga, w - glm::vec3(0.0f, 0.0f, 0.0f));
    float vb = glm::dot(gb, w - glm::vec3(1.0f, 0.0f, 0.0f));
    float vc = glm::dot(gc, w - glm::vec3(0.0f, 1.0f, 0.0f));
    float vd = glm::dot(gd, w - glm::vec3(1.0f, 1.0f, 0.0f));
    float ve = glm::dot(ge, w - glm::vec3(0.0f, 0.0f, 1.0f));
    float vf = glm::dot(gf, w - glm::vec3(1.0f, 0.0f, 1.0f));
    float vg = glm::dot(gg, w - glm::vec3(0.0f, 1.0f, 1.0f));
    float vh = glm::dot(gh, w - glm::vec3(1.0f, 1.0f, 1.0f));
	
    // interpolation
    return va + 
           u.x * (vb - va) +
           u.y * (vc - va) +
           u.z * (ve - va) +
           u.x * u.y * (va-vb-vc+vd) +
           u.y * u.z * (va-vc-ve+vg) +
           u.z * u.x * (va-vb-ve+vf) +
           u.x * u.y * u.z * (-va+vb+vc-vd+ve-vf-vg+vh);
}
