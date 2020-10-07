#pragma once
#include "Helpers.h"
#include <vector>
#include <list>
#include <algorithm>

//GL_BYTE, GL_UNSIGNED_BYTE, GL_SHORT, GL_UNSIGNED_SHORT, GL_INT, and GL_UNSIGNED_INT
template <typename FT>	inline GLenum krijgGLType			  () { return GL_FLOAT; 			}
template <> 			inline GLenum krijgGLType<char		> () { return GL_BYTE; 				}
template <> 			inline GLenum krijgGLType<short		> () { return GL_SHORT; 			}
template <> 			inline GLenum krijgGLType<uint8_t	> () { return GL_UNSIGNED_BYTE; 	}
template <> 			inline GLenum krijgGLType<int32_t	> () { return GL_INT; 				}
template <> 			inline GLenum krijgGLType<uint16_t	> () { return GL_UNSIGNED_SHORT;	}
template <> 			inline GLenum krijgGLType<uint32_t	> () { return GL_UNSIGNED_INT;		}

template <typename FT>	inline uint32_t glHappen			  () { return 4; }
template <> 			inline uint32_t glHappen< char		> () { return 1; }
template <> 			inline uint32_t glHappen< short		> () { return 2; }
template <> 			inline uint32_t glHappen< uint8_t	> () { return 1; }
template <> 			inline uint32_t glHappen< int32_t	> () { return 4; }
template <> 			inline uint32_t glHappen< uint16_t	> () { return 2; }
template <> 			inline uint32_t glHappen< uint32_t	> () { return 4; }

struct StructOfArrays
{
	StructOfArrays(GLuint ArrayBuffer, GLuint VertexAttribLocation, GLuint aantalVelden) :
		ArrayBuffer			(ArrayBuffer),
		VertexAttribLocation(VertexAttribLocation),
		aantalVelden		(aantalVelden)
		{}

	GLuint ArrayBuffer, VertexAttribLocation, aantalVelden;
};

///Stap is in het engels ook wel stride, wijzer pointer en plek in dit specifieke geval een Vertrex Attribute Location
struct stapWijzer
{
	stapWijzer(GLuint plek = 0, size_t stap = 0, size_t wijzer = 0) : plek(plek), stap(stap), wijzer(wijzer) {}

	GLuint 		plek;
	size_t		stap,
				wijzer;

};

class wrgvOpslag
{
public:
	wrgvOpslag()
	{
		glCreateVertexArrays(1, &VertexArray);
		glErrorToConsole("glCreateVertexArrays: ");
		//printf("VertexArray: %d\n", VertexArray);
	}

	template <typename veldType> int AddBufferType(int aantalVelden, std::vector<veldType> & ggvns, std::vector<stapWijzer> stapWijzers)
	{
		BindVertexArray();

		GLuint ArrayBuffer;
		glCreateBuffers(	1, 					& ArrayBuffer);
		glBindBuffer(		GL_ARRAY_BUFFER, 	ArrayBuffer);
		glBufferData(		GL_ARRAY_BUFFER, 	ggvns.size() * glHappen<veldType>(), (void *)ggvns.data(), GL_DYNAMIC_DRAW);

		glErrorToConsole("glBufferData: ");


		for(const stapWijzer & stapW : stapWijzers)
		{
			//Volgende werkt onder de aanname dat ik altijd floats gebruik als ik float wil
			if(krijgGLType<veldType>() == GL_FLOAT)	glVertexAttribPointer(	stapW.plek, aantalVelden, krijgGLType<veldType>(), false, 	stapW.stap * glHappen<veldType>(), (GLvoid *) (stapW.wijzer * glHappen<veldType>()));
			else									glVertexAttribIPointer(	stapW.plek, aantalVelden, krijgGLType<veldType>(),  		stapW.stap * glHappen<veldType>(), (GLvoid *) (stapW.wijzer * glHappen<veldType>()));

			glErrorToConsole("AddBufferType glVertexAttribPointer: ");

			glEnableVertexAttribArray(stapW.plek);
			glErrorToConsole("AddBufferType glEnableVertexAttribArray: ");

			SOAs.push_back(StructOfArrays(ArrayBuffer, stapW.plek, aantalVelden));
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glBindVertexArray(0);

		return SOAs.size() - 1;
	}

	template <typename veldType> int AddBufferType(int aantalVelden, std::vector<veldType> & ggvns, GLuint VertexAttribLocation)
	{
		return AddBufferType(aantalVelden, ggvns, { stapWijzer(VertexAttribLocation) } );
	}

	template <typename veldType> void BindNewData(int SOA, std::vector<veldType> & ggvns, bool Normalize = false)
	{
		BindVertexArray();
		glErrorToConsole("BindNewData BindVertexArray: ");

		glBindBuffer(GL_ARRAY_BUFFER, SOAs[SOA].ArrayBuffer);
		glErrorToConsole("BindNewData glBindBuffer: ");

		glBufferData(				GL_ARRAY_BUFFER, ggvns.size() * glHappen<veldType>(), (void *)ggvns.data(), GL_DYNAMIC_DRAW);
		
		//Volgende werkt onder de aanname dat ik altijd floats gebruik als ik float wil
		if(krijgGLType<veldType>() == GL_FLOAT)	glVertexAttribPointer(	SOAs[SOA].VertexAttribLocation, SOAs[SOA].aantalVelden, krijgGLType<veldType>(), Normalize, 0, 0);
		else									glVertexAttribIPointer(	SOAs[SOA].VertexAttribLocation, SOAs[SOA].aantalVelden, krijgGLType<veldType>(),			0, 0);

		glEnableVertexAttribArray(	SOAs[SOA].VertexAttribLocation);
		glErrorToConsole("BindNewData glBufferData: ");

		glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glBindVertexArray(0);
	}

	void BindVertexArray()
	{
		glBindVertexArray(VertexArray);
		glErrorToConsole("wrgvOpslag::BindVertexArray(): ");
	}

private:
	std::vector<StructOfArrays> SOAs;
	GLuint VertexArray;
};