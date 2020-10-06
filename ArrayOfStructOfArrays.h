#pragma once
#include "Helpers.h"
#include <vector>
#include <list>
#include <algorithm>

//GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT
template <typename FT>	inline GLenum GetGLType() 				{ return GL_FLOAT; 			}
template <> 			inline GLenum GetGLType<char>() 		{ return GL_BYTE; 			}
template <> 			inline GLenum GetGLType<uint8_t>() 		{ return GL_UNSIGNED_BYTE; 	}
template <> 			inline GLenum GetGLType<short>() 		{ return GL_SHORT; 			}
template <> 			inline GLenum GetGLType<uint16_t>() 	{ return GL_UNSIGNED_SHORT; }
template <> 			inline GLenum GetGLType<int32_t>() 		{ return GL_INT; 			}
template <> 			inline GLenum GetGLType<uint32_t>() 	{ return GL_UNSIGNED_INT; 	}

template <typename FT>	inline uint32_t GetGLBytes()			{ return 4; }
template <> 			inline uint32_t GetGLBytes<char>() 		{ return 1; }
template <> 			inline uint32_t GetGLBytes<uint8_t>() 	{ return 1; }
template <> 			inline uint32_t GetGLBytes<short>() 	{ return 2; }
template <> 			inline uint32_t GetGLBytes<uint16_t>() 	{ return 2; }
template <> 			inline uint32_t GetGLBytes<int32_t>() 	{ return 4; }
template <> 			inline uint32_t GetGLBytes<uint32_t>() 	{ return 4; }

struct StructOfArrays
{
	StructOfArrays(GLuint NwArrayBuffer, GLuint NwVertexAttribLocation, GLuint NwNumFields) :
		ArrayBuffer(NwArrayBuffer),
		VertexAttribLocation(NwVertexAttribLocation),
		NumFields(NwNumFields) 
		{}
		
	GLuint ArrayBuffer, VertexAttribLocation, NumFields;
};

class RenderBuffers
{
public:
	RenderBuffers()
	{
		glCreateVertexArrays(1, &VertexArray);
		glErrorToConsole("glCreateVertexArrays: ");
		//printf("VertexArray: %d\n", VertexArray);
	}
	
	template <typename TypeOfField> int AddBufferType(int NumFields, std::vector<TypeOfField> & Data, GLuint VertexAttribLocation, bool Normalize = false)
	{
		BindVertexArray();
		
		GLuint ArrayBuffer;
		glCreateBuffers(1, &ArrayBuffer);
		glBindBuffer(GL_ARRAY_BUFFER, ArrayBuffer);
		//I guess the whole 0 * NumFields etc can be removed here
		glBufferData(GL_ARRAY_BUFFER, 0 * NumFields * GetGLBytes<TypeOfField>(), (void *)Data.data(), GL_DYNAMIC_DRAW);
		glErrorToConsole("glBufferData: ");
		//printf("BufferData toegewezen\n");
		
		glEnableVertexAttribArray(VertexAttribLocation);
		glErrorToConsole("AddBufferType glEnableVertexAttribArray: ");
		
		glVertexAttribPointer(VertexAttribLocation, NumFields, GetGLType<TypeOfField>(), Normalize, 0, 0);
		glErrorToConsole("glVertexAttribPointer: ");
		//printf("VertexAttribPointer gezet\n");

		
		SOAs.push_back(StructOfArrays(ArrayBuffer, VertexAttribLocation, NumFields));
		
		//glBindVertexArray(0);
		
		return SOAs.size() - 1;
	}
	
	template <typename TypeOfField> void BindNewData(int SOA, std::vector<TypeOfField> & Data, bool Normalize = false)
	{
		BindVertexArray();
		glErrorToConsole("BindNewData BindVertexArray: ");
		
		glBindBuffer(GL_ARRAY_BUFFER, SOAs[SOA].ArrayBuffer);
		glErrorToConsole("BindNewData glBindBuffer: ");
		
		glBufferData(GL_ARRAY_BUFFER, Data.size() * GetGLBytes<TypeOfField>(), (void *)Data.data(), GL_DYNAMIC_DRAW);
		glEnableVertexAttribArray(SOAs[SOA].VertexAttribLocation);
		glVertexAttribPointer(SOAs[SOA].VertexAttribLocation, SOAs[SOA].NumFields, GetGLType<TypeOfField>(), Normalize, 0, 0);
		glErrorToConsole("BindNewData glBufferData: ");
		
		//glBindVertexArray(0);
	}
	
	void BindVertexArray()
	{
		glBindVertexArray(VertexArray);
		glErrorToConsole("RenderBuffers::BindVertexArray(): ");
	}
	
private:
	std::vector<StructOfArrays> SOAs;
	GLuint VertexArray; 
};