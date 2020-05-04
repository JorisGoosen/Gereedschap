#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "ArrayOfStructOfArrays.h"

class RenderSubBuffer
{
public:
	RenderSubBuffer(int NumFields, int Size, ArrayOfStructOfArrays *DeAOSOA, int AOSOAIndex);
	~RenderSubBuffer();
	
	int			AddDataPoint(float Val);
	int			AddDataPoint(glm::vec2 Val);	
	int			AddDataPoint(glm::vec3 Val);	
	int			AddDataPoint(glm::vec4 Val);	
	
	void		SetDataPoint(int PointIndex, float Val);
	void		SetDataPoint(int PointIndex, glm::vec2 Val);	
	void		SetDataPoint(int PointIndex, glm::vec3 Val);	
	void		SetDataPoint(int PointIndex, glm::vec4 Val);	
	
	float		GetDataPoint1(int PointIndex);
	glm::vec2	GetDataPoint2(int PointIndex);	
	glm::vec3	GetDataPoint3(int PointIndex);	
	glm::vec4	GetDataPoint4(int PointIndex);	
	
	float 	*	GetData()	{ return Data; }
	void		Flush() { Flush(GetPointIndex()); }
	void		Flush(int ZoveelDataPunten)		{ AOSOA->BindNewData(MijnAOSOAIndex, Data, ZoveelDataPunten); }
	
	int			GetPointIndex()	{ return EersteVrijPointer / MijnNumFields; }
	float*		GetDataPointer(int PointIndex) { return &(Data[PointIndex * MijnNumFields]); }

	

private:
	float	*Data = nullptr;
	int		MijnNumFields, MijnLengte, EersteVrijPointer = 0, MijnAOSOAIndex;
	
	ArrayOfStructOfArrays 	*AOSOA = nullptr;
};
