#pragma once
#include <GL/glew.h>
#include <string>
#include <GLFW/glfw3.h>


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
	
	int			ggvPuntErbij(	T 			waarde							)		;
	int			ggvPuntErbij(	glmvec2 	waarde							)		;
	int			ggvPuntErbij(	glmvec3 	waarde							)		;
	int			ggvPuntErbij(	glmvec4 	waarde							)		;
	
	void		ggvPuntZetten(	int 		PointIndex, T 			waarde	)		;
	void		ggvPuntZetten(	int 		PointIndex, glmvec2 	waarde	)		;
	void		ggvPuntZetten(	int 		PointIndex, glmvec3 	waarde	)		;
	void		ggvPuntZetten(	int 		PointIndex, glmvec4 	waarde	)		;
	
	T			ggvPunt1(		int 		PointIndex						) const	;
	glmvec2		ggvPunt2(		int 		PointIndex						) const	;	
	glmvec3		ggvPunt3(		int 		PointIndex						) const	;	
	glmvec4		ggvPunt4(		int 		PointIndex						) const	;	
	
	void		spoel()			{ _Aosoa->koppelNieuweGegevens(_adresAosoa, _ggvns); }
	size_t		grootte() const { return _ggvns.size() / _aantalVelden; } 


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

template <class T> int wrgvOnderOpslag<T>::ggvPuntErbij(T waarde)
{
	if(_aantalVelden != 1)
		throw std::invalid_argument("Trying to add single float aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
	
	_ggvns.push_back(waarde);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}

template <class T> int wrgvOnderOpslag<T>::ggvPuntErbij(glmvec2 waarde)
{
	if(_aantalVelden != 2)
		throw std::invalid_argument("Toevoegen aan glmvec2 aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns.push_back(waarde.x);
	_ggvns.push_back(waarde.y);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}

template <class T> int wrgvOnderOpslag<T>::ggvPuntErbij(glmvec3 waarde)
{
	if(_aantalVelden != 3)
		throw std::invalid_argument("Toevoegen aan glmvec3 aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns.push_back(waarde.x);
	_ggvns.push_back(waarde.y);
	_ggvns.push_back(waarde.z);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}

template <class T> int wrgvOnderOpslag<T>::ggvPuntErbij(glmvec4 waarde)
{
	if(_aantalVelden != 4)
		throw std::invalid_argument("Toevoegen aan glmvec4 aan wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns.push_back(waarde.x);
	_ggvns.push_back(waarde.y);
	_ggvns.push_back(waarde.z);
	_ggvns.push_back(waarde.w);
	
	return (_ggvns.size() - _aantalVelden) / _aantalVelden;
}


template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(int PointIndex, T waarde)
{
	if(_aantalVelden != 1)
		throw std::invalid_argument("Zetten van een T in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
	
	_ggvns[PointIndex * _aantalVelden] = waarde;
}

template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(int PointIndex, glmvec2 waarde)
{
	if(_aantalVelden != 2)
		throw std::invalid_argument("Zetten van een glmvec2 in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns[PointIndex * _aantalVelden + 0] = waarde.x;
	_ggvns[PointIndex * _aantalVelden + 1] = waarde.y;
}

template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(int PointIndex, glmvec3 waarde)
{
	if(_aantalVelden != 3)
		throw std::invalid_argument("Zetten van een glmvec3 in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns[PointIndex * _aantalVelden + 0] = waarde.x;
	_ggvns[PointIndex * _aantalVelden + 1] = waarde.y;
	_ggvns[PointIndex * _aantalVelden + 2] = waarde.z;
}

template <class T> void wrgvOnderOpslag<T>::ggvPuntZetten(int PointIndex, glmvec4 waarde)
{
	if(_aantalVelden != 4)
		throw std::invalid_argument("Zetten van een glmvec4 in wrgvOnderOpslag mislukt omdat het aantal velden benodigd het volgende is: " + std::to_string(_aantalVelden));
		
	_ggvns[PointIndex * _aantalVelden + 0] = waarde.x;
	_ggvns[PointIndex * _aantalVelden + 1] = waarde.y;
	_ggvns[PointIndex * _aantalVelden + 2] = waarde.z;
	_ggvns[PointIndex * _aantalVelden + 3] = waarde.w;
}


template <class T> T wrgvOnderOpslag<T>::ggvPunt1(int PointIndex) const
{
	if(_aantalVelden != 1)
		throw std::invalid_argument("Opvragen float uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
	
	return _ggvns[PointIndex * _aantalVelden];
}

template <class T> glm::vec<2, T, glm::defaultp> wrgvOnderOpslag<T>::ggvPunt2(int PointIndex) const
{
	if(_aantalVelden != 2)
		throw std::invalid_argument("Opvragen glmvec2 uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
		
	return glmvec2(_ggvns[PointIndex * _aantalVelden + 0], _ggvns[PointIndex * _aantalVelden + 1]);
}

template <class T> glm::vec<3, T, glm::defaultp> wrgvOnderOpslag<T>::ggvPunt3(int PointIndex) const
{
	if(_aantalVelden != 3)
		throw std::invalid_argument("Opvragen glmvec3 uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
		
	return glmvec3(_ggvns[PointIndex * _aantalVelden + 0], _ggvns[PointIndex * _aantalVelden + 1], _ggvns[PointIndex * _aantalVelden + 2]);
}

template <class T> glm::vec<4, T, glm::defaultp> wrgvOnderOpslag<T>::ggvPunt4(int PointIndex) const
{
	if(_aantalVelden != 4)
		throw std::invalid_argument("Opvragen glmvec4 uit wrgvOnderOpslag mislukt want aantal velden : " + std::to_string(_aantalVelden));
		
	return glmvec4(_ggvns[PointIndex * _aantalVelden + 0], _ggvns[PointIndex * _aantalVelden + 1], _ggvns[PointIndex * _aantalVelden + 2], _ggvns[PointIndex * _aantalVelden + 3]);
}