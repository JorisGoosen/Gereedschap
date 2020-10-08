#pragma once
#include "helpers.h"
#include <map>
#include <list>
#include <vector>
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

///Bevat de benodigde informatie voor een punt-eigenschap-plek (vertex attribute location) binnen een puntreeks (vertex array)
struct puntEigenschapPlekInfo
{
	puntEigenschapPlekInfo(GLuint reeksOpslag = 0, GLuint puntEigenschapPlek = 0, GLuint aantalVelden = 0) :
		reeksOpslag			(reeksOpslag),
		puntEigenschapPlek	(puntEigenschapPlek),
		aantalVelden		(aantalVelden)
		{}

	GLuint reeksOpslag, puntEigenschapPlek, aantalVelden;
};

///Stap is in het engels ook wel stride, wijzer pointer en plek in dit specifieke geval een Vertrex Attribute Location
struct stapWijzer
{
	stapWijzer(GLuint plek = 0, size_t stap = 0, size_t wijzer = 0) 
	: plek(plek), stap(stap), wijzer(wijzer) {}

	GLuint 		plek;
	size_t		stap,
				wijzer;

};

class wrgvOpslag
{
public:
	wrgvOpslag()
	{
		glCreateVertexArrays(1, &_reeksOpslag);
		glErrorToConsole("glCreateVertexArrays: ");
	}

	GLuint reeksBufferVoorPlek(GLuint plek)	{ return _plekInfos.at(plek).reeksOpslag; }

	template <typename veldType> GLuint reeksOpslagErbij(int aantalVelden, std::vector<veldType> & ggvns, std::vector<stapWijzer> stapWijzers)
	{
		BindVertexArray();

		GLuint reeksOpslag;
		glCreateBuffers(	1, 					& reeksOpslag);
		glBindBuffer(		GL_ARRAY_BUFFER, 	reeksOpslag);
		glBufferData(		GL_ARRAY_BUFFER, 	ggvns.size() * glHappen<veldType>(), (void *)ggvns.data(), GL_DYNAMIC_DRAW);

		glErrorToConsole("glBufferData: ");


		for(const stapWijzer & stapW : stapWijzers)
		{
			//Volgende werkt onder de aanname dat ik altijd floats gebruik als ik float wil
			if(krijgGLType<veldType>() == GL_FLOAT)	glVertexAttribPointer(	stapW.plek, aantalVelden, krijgGLType<veldType>(), false, 	stapW.stap * glHappen<veldType>(), (GLvoid *) (stapW.wijzer * glHappen<veldType>()));
			else									glVertexAttribIPointer(	stapW.plek, aantalVelden, krijgGLType<veldType>(),  		stapW.stap * glHappen<veldType>(), (GLvoid *) (stapW.wijzer * glHappen<veldType>()));

			glErrorToConsole("reeksOpslagErbij glVertexAttribPointer: ");

			glEnableVertexAttribArray(stapW.plek);
			glErrorToConsole("reeksOpslagErbij glEnableVertexAttribArray: ");

			_plekInfos[stapW.plek] = puntEigenschapPlekInfo(reeksOpslag, stapW.plek, aantalVelden);
		}

		glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glBindVertexArray(0);

		return reeksOpslag;
	}

	template <typename veldType> GLuint reeksOpslagErbij(int aantalVelden, std::vector<veldType> & ggvns, GLuint puntEigenschapPlek)
	{
		return reeksOpslagErbij(aantalVelden, ggvns, { stapWijzer(puntEigenschapPlek) } );
	}

	template <typename veldType> void koppelNieuweGegevens(int plek, std::vector<veldType> & ggvns)
	{
		BindVertexArray();
		glErrorToConsole("koppelNieuweGegevens BindVertexArray: ");

		glBindBuffer(GL_ARRAY_BUFFER, _plekInfos[plek].reeksOpslag);
		glErrorToConsole("koppelNieuweGegevens glBindBuffer: ");

		glBufferData(				GL_ARRAY_BUFFER, ggvns.size() * glHappen<veldType>(), (void *)ggvns.data(), GL_DYNAMIC_DRAW);
		
		//Volgende werkt onder de aanname dat ik altijd floats gebruik als ik float wil
		if(krijgGLType<veldType>() == GL_FLOAT)	glVertexAttribPointer(	_plekInfos[plek].puntEigenschapPlek, _plekInfos[plek].aantalVelden, krijgGLType<veldType>(), false,	0, 0);
		else									glVertexAttribIPointer(	_plekInfos[plek].puntEigenschapPlek, _plekInfos[plek].aantalVelden, krijgGLType<veldType>(),			0, 0);

		glEnableVertexAttribArray(_plekInfos[plek].puntEigenschapPlek);
		glErrorToConsole("koppelNieuweGegevens glBufferData: ");

		glBindBuffer(GL_ARRAY_BUFFER, 0);
    	glBindVertexArray(0);
	}

	void BindVertexArray()
	{
		glBindVertexArray(_reeksOpslag);
		glErrorToConsole("wrgvOpslag::BindVertexArray(): ");
	}

private:
	/// puntEigenschapPlekken ->  puntEigenschapPlekInfo
	std::map<GLuint, puntEigenschapPlekInfo> 	_plekInfos;	
	GLuint 										_reeksOpslag;
};