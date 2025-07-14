#pragma once
#include "../wrgvOnderOpslag.h"
#include <filesystem>


class plyLezer
{
public:
	plyLezer();

	bool openPlyBestand(const std::filesystem::path & plyfile);

	///Teken jezelf met puntjes
	void tekenJezelf() 			const;

	void bindPuntReeks() const { _reeks->bindPuntReeks();	 }
	
protected:
	int								_elementen	= -1;

	wrgvOpslag						* _reeks  	= nullptr;
	wrgvOnderOpslag<float>			* _xyz 		= nullptr;
	wrgvOnderOpslag<float>			* _nxyz 	= nullptr;
	
};