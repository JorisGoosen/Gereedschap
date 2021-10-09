#include "../wrgvOnderOpslag.h"


class vierkantRooster
{
public:
	vierkantRooster(size_t breedte, size_t hoogte);

	///Teken jezelf met driehoeken
	virtual void tekenJezelf() 			const; 

	///Teken jezelf met patches, tbv vlakverdeling
	virtual void tekenJezelfPatchy() 	const;

	void bindPuntReeks() const { _reeks->bindPuntReeks();	 }
	
protected:
	void genereer();

	wrgvOpslag						* 	_reeks  = nullptr;
	wrgvOnderOpslag<float>			* 	_punten = nullptr;
	std::vector<glm::uint32>	  	  	_vierkanten;
	size_t								_breedte,
										_hoogte;
};