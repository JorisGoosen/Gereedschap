#pragma once
#include <GL/glew.h>
#include <string>
#include <GLFW/glfw3.h>


#include "ArrayOfStructOfArrays.h"


template <class T>
class wrgvOnderOpslag
{
public:
	typedef glm::vec<2, T, glm::defaultp> glmvec2;
	typedef glm::vec<3, T, glm::defaultp> glmvec3;
	typedef glm::vec<4, T, glm::defaultp> glmvec4;

				wrgvOnderOpslag(int NumFields, wrgvOpslag *DeAOSOA, int AOSOAIndex);
				~wrgvOnderOpslag();
	
	int			AddDataPoint(T Val);
	int			AddDataPoint(glmvec2 Val);	
	int			AddDataPoint(glmvec3 Val);	
	int			AddDataPoint(glmvec4 Val);	
	
	void		SetDataPoint(int PointIndex, T Val);
	void		SetDataPoint(int PointIndex, glmvec2 Val);	
	void		SetDataPoint(int PointIndex, glmvec3 Val);	
	void		SetDataPoint(int PointIndex, glmvec4 Val);	
	
	T			GetDataPoint1(int PointIndex) const;
	glmvec2		GetDataPoint2(int PointIndex) const;	
	glmvec3		GetDataPoint3(int PointIndex) const;	
	glmvec4		GetDataPoint4(int PointIndex) const;	
	
//	T 		*	GetData()						{ return Data.data(); }
	void		Flush()							{ AOSOA->BindNewData(MijnAOSOAIndex, Data); }
	
	//T*			GetDataPointer(int PointIndex) { return &(Data[PointIndex * MijnNumFields]); }

	size_t		Aantal() const { return Data.size() / MijnNumFields; } 


private:
	std::vector<T> 		Data;
	int					MijnNumFields, 
						MijnAOSOAIndex;
	
	wrgvOpslag 	*	AOSOA = nullptr;
};


template <class T> wrgvOnderOpslag<T>::wrgvOnderOpslag(int NumFields, wrgvOpslag *DeAOSOA, int AOSOAIndex)
{
	MijnNumFields	= NumFields;
	AOSOA			= DeAOSOA;
	MijnAOSOAIndex	= AOSOAIndex;
	
	AOSOA->AddBufferType(MijnNumFields, Data, MijnAOSOAIndex);
}

template <class T> wrgvOnderOpslag<T>::~wrgvOnderOpslag()
{ 
	Data.clear();
}

template <class T> int wrgvOnderOpslag<T>::AddDataPoint(T Val)
{
	if(MijnNumFields != 1)
		throw std::invalid_argument("Trying to add single float to wrgvOnderOpslag but NumFields is WRONG");
	
	Data.push_back(Val);
	
	return (Data.size() - MijnNumFields) / MijnNumFields;
}

template <class T> int wrgvOnderOpslag<T>::AddDataPoint(glmvec2 Val)
{
	if(MijnNumFields != 2)
		throw std::invalid_argument("Trying to add single glmvec2 to wrgvOnderOpslag but NumFields is WRONG");
		
	Data.push_back(Val.x);
	Data.push_back(Val.y);
	
	return (Data.size() - MijnNumFields) / MijnNumFields;
}

template <class T> int wrgvOnderOpslag<T>::AddDataPoint(glmvec3 Val)
{
	if(MijnNumFields != 3)
		throw std::invalid_argument("Trying to add single glmvec3 to wrgvOnderOpslag but NumFields is WRONG");
		
	Data.push_back(Val.x);
	Data.push_back(Val.y);
	Data.push_back(Val.z);
	
	return (Data.size() - MijnNumFields) / MijnNumFields;
}

template <class T> int wrgvOnderOpslag<T>::AddDataPoint(glmvec4 Val)
{
	if(MijnNumFields != 4)
		throw std::invalid_argument("Trying to add single glmvec4 to wrgvOnderOpslag but NumFields is WRONG");
		
	Data.push_back(Val.x);
	Data.push_back(Val.y);
	Data.push_back(Val.z);
	Data.push_back(Val.w);
	
	return (Data.size() - MijnNumFields) / MijnNumFields;
}


template <class T> void wrgvOnderOpslag<T>::SetDataPoint(int PointIndex, T Val)
{
	if(MijnNumFields != 1)
		throw std::invalid_argument("Trying to set single T to wrgvOnderOpslag but NumFields is WRONG");
	
	Data[PointIndex * MijnNumFields] = Val;
}

template <class T> void wrgvOnderOpslag<T>::SetDataPoint(int PointIndex, glmvec2 Val)
{
	if(MijnNumFields != 2)
		throw std::invalid_argument("Trying to set single glmvec2 to wrgvOnderOpslag but NumFields is WRONG");
		
	Data[PointIndex * MijnNumFields + 0] = Val.x;
	Data[PointIndex * MijnNumFields + 1] = Val.y;
}

template <class T> void wrgvOnderOpslag<T>::SetDataPoint(int PointIndex, glmvec3 Val)
{
	if(MijnNumFields != 3)
		throw std::invalid_argument("Trying to set single glmvec3 to wrgvOnderOpslag but NumFields is WRONG");
		
	Data[PointIndex * MijnNumFields + 0] = Val.x;
	Data[PointIndex * MijnNumFields + 1] = Val.y;
	Data[PointIndex * MijnNumFields + 2] = Val.z;
}

template <class T> void wrgvOnderOpslag<T>::SetDataPoint(int PointIndex, glmvec4 Val)
{
	if(MijnNumFields != 4)
		throw std::invalid_argument("Trying to set single glmvec4 to wrgvOnderOpslag but NumFields is WRONG");
		
	Data[PointIndex * MijnNumFields + 0] = Val.x;
	Data[PointIndex * MijnNumFields + 1] = Val.y;
	Data[PointIndex * MijnNumFields + 2] = Val.z;
	Data[PointIndex * MijnNumFields + 3] = Val.w;
}

template <class T> T wrgvOnderOpslag<T>::GetDataPoint1(int PointIndex) const
{
	if(MijnNumFields != 1)
		throw std::invalid_argument("Trying to get single float from wrgvOnderOpslag but NumFields is WRONG");
	
	return Data[PointIndex * MijnNumFields];
}

template <class T> glm::vec<2, T, glm::defaultp> wrgvOnderOpslag<T>::GetDataPoint2(int PointIndex) const
{
	if(MijnNumFields != 2)
		throw std::invalid_argument("Trying to get single glmvec2 from wrgvOnderOpslag but NumFields is WRONG");
		
	return glmvec2(Data[PointIndex * MijnNumFields + 0], Data[PointIndex * MijnNumFields + 1]);
}

template <class T> glm::vec<3, T, glm::defaultp> wrgvOnderOpslag<T>::GetDataPoint3(int PointIndex) const
{
	if(MijnNumFields != 3)
		throw std::invalid_argument("Trying to get single glmvec3 from wrgvOnderOpslag but NumFields is WRONG");
		
	return glmvec3(Data[PointIndex * MijnNumFields + 0], Data[PointIndex * MijnNumFields + 1], Data[PointIndex * MijnNumFields + 2]);
}

template <class T> glm::vec<4, T, glm::defaultp> wrgvOnderOpslag<T>::GetDataPoint4(int PointIndex) const
{
	if(MijnNumFields != 4)
		throw std::invalid_argument("Trying to get single glmvec4 from wrgvOnderOpslag but NumFields is WRONG");
		
	return glmvec4(Data[PointIndex * MijnNumFields + 0], Data[PointIndex * MijnNumFields + 1], Data[PointIndex * MijnNumFields + 2], Data[PointIndex * MijnNumFields + 3]);
}
