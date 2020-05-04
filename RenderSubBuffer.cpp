#include "RenderSubBuffer.h"

using namespace glm;

RenderSubBuffer::RenderSubBuffer(int NumFields, int Size, ArrayOfStructOfArrays *DeAOSOA, int AOSOAIndex)
{
	MijnNumFields	= NumFields;
	MijnLengte		= Size;
	Data			= new float[MijnLengte * MijnNumFields]();
	AOSOA			= DeAOSOA;
	MijnAOSOAIndex	= AOSOAIndex;
	
	AOSOA->AddBufferType(MijnNumFields, Data, MijnAOSOAIndex);
}

RenderSubBuffer::~RenderSubBuffer()
{ 
	if(Data != NULL) 
		delete Data; 
	
	Data = NULL;
}

int RenderSubBuffer::AddDataPoint(float Val)
{
	if(MijnNumFields != 1)
		throw std::invalid_argument("Trying to add single float to RenderSubBuffer but NumFields is WRONG");
	
	Data[EersteVrijPointer++] = Val;
	
	return (EersteVrijPointer - MijnNumFields) / MijnNumFields;
}

int RenderSubBuffer::AddDataPoint(vec2 Val)
{
	if(MijnNumFields != 2)
		throw std::invalid_argument("Trying to add single vec2 to RenderSubBuffer but NumFields is WRONG");
		
	Data[EersteVrijPointer++] = Val.x;
	Data[EersteVrijPointer++] = Val.y;
	
	return (EersteVrijPointer - MijnNumFields) / MijnNumFields;
}

int RenderSubBuffer::AddDataPoint(vec3 Val)
{
	if(MijnNumFields != 3)
		throw std::invalid_argument("Trying to add single vec3 to RenderSubBuffer but NumFields is WRONG");
		
	Data[EersteVrijPointer++] = Val.x;
	Data[EersteVrijPointer++] = Val.y;
	Data[EersteVrijPointer++] = Val.z;
	
	return (EersteVrijPointer - MijnNumFields) / MijnNumFields;
}

int RenderSubBuffer::AddDataPoint(vec4 Val)
{
	if(MijnNumFields != 4)
		throw std::invalid_argument("Trying to add single vec4 to RenderSubBuffer but NumFields is WRONG");
		
	Data[EersteVrijPointer++] = Val.x;
	Data[EersteVrijPointer++] = Val.y;
	Data[EersteVrijPointer++] = Val.z;
	Data[EersteVrijPointer++] = Val.w;
	
	return (EersteVrijPointer - MijnNumFields) / MijnNumFields;
}


void RenderSubBuffer::SetDataPoint(int PointIndex, float Val)
{
	if(MijnNumFields != 1)
		throw std::invalid_argument("Trying to set single float to RenderSubBuffer but NumFields is WRONG");
	
	Data[PointIndex * MijnNumFields] = Val;
}

void RenderSubBuffer::SetDataPoint(int PointIndex, vec2 Val)
{
	if(MijnNumFields != 2)
		throw std::invalid_argument("Trying to set single vec2 to RenderSubBuffer but NumFields is WRONG");
		
	Data[PointIndex * MijnNumFields + 0] = Val.x;
	Data[PointIndex * MijnNumFields + 1] = Val.y;
}

void RenderSubBuffer::SetDataPoint(int PointIndex, vec3 Val)
{
	if(MijnNumFields != 3)
		throw std::invalid_argument("Trying to set single vec3 to RenderSubBuffer but NumFields is WRONG");
		
	Data[PointIndex * MijnNumFields + 0] = Val.x;
	Data[PointIndex * MijnNumFields + 1] = Val.y;
	Data[PointIndex * MijnNumFields + 2] = Val.z;
}

void RenderSubBuffer::SetDataPoint(int PointIndex, vec4 Val)
{
	if(MijnNumFields != 4)
		throw std::invalid_argument("Trying to set single vec4 to RenderSubBuffer but NumFields is WRONG");
		
	Data[PointIndex * MijnNumFields + 0] = Val.x;
	Data[PointIndex * MijnNumFields + 1] = Val.y;
	Data[PointIndex * MijnNumFields + 2] = Val.z;
	Data[PointIndex * MijnNumFields + 3] = Val.w;
}

float RenderSubBuffer::GetDataPoint1(int PointIndex)
{
	if(MijnNumFields != 1)
		throw std::invalid_argument("Trying to get single float from RenderSubBuffer but NumFields is WRONG");
	
	return Data[PointIndex * MijnNumFields];
}

vec2 RenderSubBuffer::GetDataPoint2(int PointIndex)
{
	if(MijnNumFields != 2)
		throw std::invalid_argument("Trying to get single vec2 from RenderSubBuffer but NumFields is WRONG");
		
	return vec2(Data[PointIndex * MijnNumFields + 0], Data[PointIndex * MijnNumFields + 1]);
}

vec3 RenderSubBuffer::GetDataPoint3(int PointIndex)
{
	if(MijnNumFields != 3)
		throw std::invalid_argument("Trying to get single vec3 from RenderSubBuffer but NumFields is WRONG");
		
	return vec3(Data[PointIndex * MijnNumFields + 0], Data[PointIndex * MijnNumFields + 1], Data[PointIndex * MijnNumFields + 2]);
}

vec4 RenderSubBuffer::GetDataPoint4(int PointIndex)
{
	if(MijnNumFields != 4)
		throw std::invalid_argument("Trying to get single vec4 from RenderSubBuffer but NumFields is WRONG");
		
	return vec4(Data[PointIndex * MijnNumFields + 0], Data[PointIndex * MijnNumFields + 1], Data[PointIndex * MijnNumFields + 2], Data[PointIndex * MijnNumFields + 3]);
}
