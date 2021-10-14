#include "../wrgvOnderOpslag.h"


class vierkantRooster
{
public:
	vierkantRooster(size_t breedte, size_t hoogte, float schaling = 1.0f);

	///Teken jezelf met driehoeken
	virtual void tekenJezelf() 			const; 

	///Teken jezelf met patches, tbv vlakverdeling
	virtual void tekenJezelfPatchy() 	const;

	void bindPuntReeks() const { _reeks->bindPuntReeks();	 }

	float schaling() const { return _schaling; }
	
protected:
	void genereer();

	wrgvOpslag						* 	_reeks  	= nullptr;
	wrgvOnderOpslag<float>			* 	_punten 	= nullptr,
									*	_texturen	= nullptr;
	std::vector<glm::uint32>	  	  	_vierkanten;
	size_t								_breedte,
										_hoogte;
	float								_schaling;
};