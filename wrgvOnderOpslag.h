#pragma once
#include <GL/glew.h>
#include <string>
#include <GLFW/glfw3.h>
#include <array>

#include "wrgvOpslag.h"


template <class T>
class wrgvOnderOpslag
{
public:
	typedef glm::vec<2, T, glm::defaultp> 	glmvec2;
	typedef glm::vec<3, T, glm::defaultp> 	glmvec3;
	typedef glm::vec<4, T, glm::defaultp> 	glmvec4;

				wrgvOnderOpslag(int aantalVelden, wrgvOpslag *DeAOSOA, int AOSOAIndex);
				~wrgvOnderOpslag();
	
	size_t		ggvPuntErbij(	T 			waarde							)		;
	size_t		ggvPuntErbij(	glmvec2 	waarde							)		;
	size_t		ggvPuntErbij(	glmvec3 	waarde							)		;
	size_t		ggvPuntErbij(	glmvec4 	waarde							)		;
	
	void		ggvPuntZetten(	size_t 		puntAdres, T 		waarde		)		;
	void		ggvPuntZetten(	size_t 		puntAdres, glmvec2 	waarde		)		;
	void		ggvPuntZetten(	size_t 		puntAdres, glmvec3 	waarde		)		;
	void		ggvPuntZetten(	size_t 		puntAdres, glmvec4 	waarde		)		;
	
	T			ggvPunt1(		size_t 		puntAdres						) const	;
	glmvec2		ggvPunt2(		size_t 		puntAdres						) const	;	
	glmvec3		ggvPunt3(		size_t 		puntAdres						) const	;	
	glmvec4		ggvPunt4(		size_t 		puntAdres						) const	;	
	
	void		spoel()			{ _Aosoa->koppelNieuweGegevens(_adresAosoa, _ggvns); }
	size_t		grootte() const { return _ggvns.size() / _aantalVelden; } 

	void		zetGegevens(const std::vector<T>		& ggvnsNieuw) { _ggvns = ggvnsNieuw; spoel(); }
	void		zetGegevens(const std::vector<glmvec2>	& ggvnsNieuw);
	void		zetGegevens(const std::vector<glmvec3>	& ggvnsNieuw);
	void		zetGegevens(const std::vector<glmvec4>	& ggvnsNieuw);


	std::vector<T>			gegevens1() const	{ return _ggvns; }
	std::vector<glmvec2>	gegevens2() const;
	std::vector<glmvec3>	gegevens3() const;
	std::vector<glmvec4>	gegevens4() const;

private:
	std::vector<T> 		_ggvns;
	int					_aantalVelden, 
						_adresAosoa;
	
	wrgvOpslag 		*	_Aosoa = nullptr;
};


template <class T> wrgvOnderOpslag<T>::wrgvOnderOpslag(int aantalVelden, wrgvOpslag *DeAOSOA, int AOSOAIndex)
{
	_aantalVelden	= aantalVelden;
	_Aosoa			= DeAOSOA;
	_adresAosoa		= AOSOAIndex;
	
	_Aosoa->reeksOpslagErbij(_aantalVelden, _ggvns, _adresAosoa);
}

template <class T> wrgvOnderOpslag<T>::~wrgvOnderOpslag()
{ 
	_ggvns.clear();
}

template <class T> size_t wrgvOnderOpslag<T>::ggvPuntErbij(T waarde)
{
	if(_aantalVelden != 1)
		throw std::invalid_argument("Trying to add single float aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
	
	_ggvns.push_back(waarde);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}

template <class T> size_t wrgvOnderOpslag<T>::ggvPuntErbij(glmvec2 waarde)
{
	if(_aantalVelden != 2)
		throw std::invalid_argument("Toevoegen aan glmvec2 aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns.push_back(waarde.x);
	_ggvns.push_back(waarde.y);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}

template <class T> size_t wrgvOnderOpslag<T>::ggvPuntErbij(glmvec3 waarde)
{
	if(_aantalVelden != 3)
		throw std::invalid_argument("Toevoegen aan glmvec3 aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns.push_back(waarde.x);
	_ggvns.push_back(waarde.y);
	_ggvns.push_back(waarde.z);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}

template <class T> size_t wrgvOnderOpslag<T>::ggvPuntErbij(glmvec4 waarde)
{
	if(_aantalVelden != 4)
		throw std::invalid_argument("Toevoegen aan glmvec4 aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns.push_back(waarde.x);
	_ggvns.push_back(waarde.y);
	_ggvns.push_back(waarde.z);
	_ggvns.push_back(waarde.w);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}


template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(size_t puntAdres, T waarde)
{
	if(_aantalVelden != 1)
		throw std::invalid_argument("Zetten van een T in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
	
	_ggvns[puntAdres * _aantalVelden] = waarde;
}

template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(size_t puntAdres, glmvec2 waarde)
{
	if(_aantalVelden != 2)
		throw std::invalid_argument("Zetten van een glmvec2 in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns[puntAdres * _aantalVelden + 0] = waarde.x;
	_ggvns[puntAdres * _aantalVelden + 1] = waarde.y;
}

template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(size_t puntAdres, glmvec3 waarde)
{
	if(_aantalVelden != 3)
		throw std::invalid_argument("Zetten van een glmvec3 in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns[puntAdres * _aantalVelden + 0] = waarde.x;
	_ggvns[puntAdres * _aantalVelden + 1] = waarde.y;
	_ggvns[puntAdres * _aantalVelden + 2] = waarde.z;
}

template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(size_t puntAdres, glmvec4 waarde)
{
	if(_aantalVelden != 4)
		throw std::invalid_argument("Zetten van een glmvec4 in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns[puntAdres * _aantalVelden + 0] = waarde.x;
	_ggvns[puntAdres * _aantalVelden + 1] = waarde.y;
	_ggvns[puntAdres * _aantalVelden + 2] = waarde.z;
	_ggvns[puntAdres * _aantalVelden + 3] = waarde.w;
}


template <class T> T wrgvOnderOpslag<T>::ggvPunt1(size_t puntAdres) const
{
	if(_aantalVelden != 1)
		throw std::invalid_argument("Opvragen float uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
	
	return _ggvns[puntAdres * _aantalVelden];
}

template <class T> glm::vec<2, T, glm::defaultp> wrgvOnderOpslag<T>::ggvPunt2(size_t puntAdres) const
{
	if(_aantalVelden != 2)
		throw std::invalid_argument("Opvragen glmvec2 uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
		
	return glmvec2(_ggvns[puntAdres * _aantalVelden + 0], _ggvns[puntAdres * _aantalVelden + 1]);
}

template <class T> glm::vec<3, T, glm::defaultp> wrgvOnderOpslag<T>::ggvPunt3(size_t puntAdres) const
{
	if(_aantalVelden != 3)
		throw std::invalid_argument("Opvragen glmvec3 uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
		
	return glmvec3(_ggvns[puntAdres * _aantalVelden + 0], _ggvns[puntAdres * _aantalVelden + 1], _ggvns[puntAdres * _aantalVelden + 2]);
}

template <class T> glm::vec<4, T, glm::defaultp> wrgvOnderOpslag<T>::ggvPunt4(size_t puntAdres) const
{
	if(_aantalVelden != 4)
		throw std::invalid_argument("Opvragen glmvec4 uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
		
	return glmvec4(_ggvns[puntAdres * _aantalVelden + 0], _ggvns[puntAdres * _aantalVelden + 1], _ggvns[puntAdres * _aantalVelden + 2], _ggvns[puntAdres * _aantalVelden + 3]);
}


//Ik zou hier misschien een macro of rare template vann kunnen maken maar daar wordt het vast niet leesbaarder van
template <class T> std::vector<glm::vec<2, T, glm::defaultp>>	wrgvOnderOpslag<T>::gegevens2() const
{
	std::vector<glm::vec<2, T, glm::defaultp>> lijst; 

	size_t grrrr = grootte();
	lijst.resize(grrrr);

	for(size_t i=0; i<grrrr; i++)
		lijst[i] = ggvPunt2(i);

	return lijst;
}

template <class T> std::vector<glm::vec<3, T, glm::defaultp>>	wrgvOnderOpslag<T>::gegevens3() const
{
	std::vector<glm::vec<3, T, glm::defaultp>> lijst; 

	size_t grrrr = grootte();
	lijst.resize(grrrr);

	for(size_t i=0; i<grrrr; i++)
		lijst[i] = ggvPunt3(i);

	return lijst;
}

template <class T> std::vector<glm::vec<4, T, glm::defaultp>>	wrgvOnderOpslag<T>::gegevens4() const
{
	std::vector<glm::vec<4, T, glm::defaultp>> lijst; 

	size_t grrrr = grootte();
	lijst.resize(grrrr);

	for(size_t i=0; i<grrrr; i++)
		lijst[i] = ggvPunt4(i);

	return lijst;
}


template <class T> void	wrgvOnderOpslag<T>::zetGegevens(const std::vector<glm::vec<2, T, glm::defaultp>>	& ggvnsNieuw)
{
	_ggvns.resize(ggvnsNieuw.size() * _aantalVelden);

	for(size_t i=0; i<ggvnsNieuw.size(); i++)
		ggvPuntZetten(i, ggvnsNieuw[i]);

	spoel();
}

template <class T> void	wrgvOnderOpslag<T>::zetGegevens(const std::vector<glm::vec<3, T, glm::defaultp>>	& ggvnsNieuw)
{
	_ggvns.resize(ggvnsNieuw.size() * _aantalVelden);

	for(size_t i=0; i<ggvnsNieuw.size(); i++)
		ggvPuntZetten(i, ggvnsNieuw[i]);

	spoel();
}

template <class T> void	wrgvOnderOpslag<T>::zetGegevens(const std::vector<glm::vec<4, T, glm::defaultp>>	& ggvnsNieuw)
{
	_ggvns.resize(ggvnsNieuw.size() * _aantalVelden);

	for(size_t i=0; i<ggvnsNieuw.size(); i++)
		ggvPuntZetten(i, ggvnsNieuw[i]);

	spoel();
}