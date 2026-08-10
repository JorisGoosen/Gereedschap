#pragma once

#include <webgpu.h>
#include <string>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "png.h"
#include <ostream>
#include <iomanip>
#include <sstream>

///Controleert of het wgpu-apparaat fouten heeft gemeld en meldt die dan; bij echte fouten wordt gegooid
void wgpFoutControle(const std::string & huidigeActie = "");

///Wordt aangeroepen door de foutmelding-callback van het apparaat om een fout te registreren
void meldWgpFout(const std::string & boodschap);

///Maakt een WGSL shader module van het bestand (met #include verwerking) en gooit bij compileerfouten
WGPUShaderModule _maakShaderModule(const std::string & shaderBestandsnaam, WGPUDevice apparaat);

///Leest een bestand en vouwt "#include \"bestand\"" regels in (net als vroeger in OpenGL)
std::string tekstInlezen(const std::string & bestandsNaam);

		glm::vec3	willekeurigeVec3();
inline 	glm::vec3	willekeurigeVec3Z() { return glm::vec3(-1.0f) + (2.0f * willekeurigeVec3()); }

png_byte *	laadPNG(const std::string & bestandsnaam, size_t & width, size_t & height, size_t & kanalen);

///WebGPU kent geen geometry- of vlak-verdelings shaders (alleen de reken en vertex/fragment)
[[noreturn]] void werpOnondersteund(const std::string & wat);


inline std::string		_doubleStr(double number) 
{ 
	const size_t maxSize = 6;
	std::stringstream str; 
	str  << (number >= 0 ? " " : "") << std::setw(maxSize - 4) << std::setfill(' ') << std::showpoint << number; 
	
	while(str.str().size() < maxSize) 
		str << std::setw(1) << ' '; 
	
	return str.str().substr(0, maxSize);  //just drop the rest
}

inline std::ostream& 	operator<<(std::ostream& os, const glm::vec2 & vec) { return (os << "[ " << _doubleStr(vec.x) << ", " << _doubleStr(vec.y)																<< " ]"); }
inline std::ostream& 	operator<<(std::ostream& os, const glm::vec3 & vec) { return (os << "[ " << _doubleStr(vec.x) << ", " << _doubleStr(vec.y) << ", " << _doubleStr(vec.z) 								<< " ]"); }
inline std::ostream& 	operator<<(std::ostream& os, const glm::vec4 & vec) { return (os << "[ " << _doubleStr(vec.x) << ", " << _doubleStr(vec.y) << ", " << _doubleStr(vec.z) << ", " << _doubleStr(vec.w) 	<< " ]"); }