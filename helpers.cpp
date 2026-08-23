#include "helpers.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <exception>
#include <random>
#include <regex>
#include <filesystem>

static bool 				wgpHadFout 			= false;
static std::string 			wgpLaatsteFoutBoodschap;

void meldWgpFout(const std::string & boodschap)
{
	wgpHadFout 				= true;
	wgpLaatsteFoutBoodschap	= boodschap;
	std::cout << "WebGPU: " << boodschap << std::endl;
}

void wgpFoutControle(const std::string & huidigeActie)
{
	if(!wgpHadFout)
		return;

	wgpHadFout = false;

	if(huidigeActie != "")
		std::cout << huidigeActie << ": ";

	std::cout << wgpLaatsteFoutBoodschap << std::endl;

	wgpLaatsteFoutBoodschap.clear();

	throw std::runtime_error("Er is een WebGPU fout opgetreden...");
}


std::string tekstInlezen(const std::string & bestandsNaam) 
{
	using namespace std::filesystem;
	path bestand(bestandsNaam);

	std::ifstream leesShader(bestandsNaam);

	if(!leesShader.is_open())
		throw std::runtime_error("Het openen van de shader " + bestandsNaam + " is helaas niet gelukt...");

	std::regex headers("\\s*#include\\s+\"(\\w+\\.\\w+)\"\\s*");
	
	std::stringstream 	touw;
	std::string			regel;
	std::smatch 		m;

	while(std::getline(leesShader, regel))
		touw << (std::regex_match(regel, m, headers) ? tekstInlezen(bestand.replace_filename(path(m[1])).string()) : regel) << "\n";

	if(false)
	{
		const char * streepjes = "-----------------";
		std::cout << "\n" << streepjes << "tekstInlezen('" << bestandsNaam << "')" << streepjes << "\n" << touw.str() << streepjes << "\nEinde '" << bestandsNaam << "'" << streepjes <<  std::endl;
	}
	
	return touw.str();
}


void werpOnondersteund(const std::string & wat)
{
	throw std::runtime_error("Nog niet ondersteund in WebGPU: " + wat);
}


WGPUShaderModule _maakShaderModule(const std::string & shaderBestandsnaam, WGPUDevice apparaat)
{
	std::string shaderBron = tekstInlezen(shaderBestandsnaam);

	WGPUShaderSourceWGSL wgslBron = WGPU_SHADER_SOURCE_WGSL_INIT;
	wgslBron.code.data 	= shaderBron.c_str();
	wgslBron.code.length = shaderBron.size();

	WGPUShaderModuleDescriptor beschrijving = WGPU_SHADER_MODULE_DESCRIPTOR_INIT;
	beschrijving.nextInChain = &wgslBron.chain;
	beschrijving.label		= { shaderBestandsnaam.c_str(), shaderBestandsnaam.size() };

	std::cout << "Compiling " << shaderBestandsnaam << "..." << std::endl;

	WGPUShaderModule module = wgpuDeviceCreateShaderModule(apparaat, &beschrijving);

	if(!module)
	{
		meldWgpFout("Het aanmaken van de shader module '" + shaderBestandsnaam + "' is mislukt (controleer de WGSL!)");
		throw std::runtime_error("Shader module aanmaken mislukt: " + shaderBestandsnaam);
	}

	wgpFoutControle("_maakShaderModule('" + shaderBestandsnaam + "'): ");

	return module;
}


glm::vec3 willekeurigeVec3()
{
	static std::random_device					willekeur;  
    static std::mt19937 						bemonsteraar(willekeur());
	static std::uniform_real_distribution<> 	greep(-1.0, 1.0);

	return glm::vec3(greep(bemonsteraar), greep(bemonsteraar), greep(bemonsteraar));
}


unsigned char *	laadAfbeelding(const std::string & bestandsnaam, size_t & width, size_t & height, size_t & kanalen)
{
	int w, h, n;
	unsigned char *data = stbi_load(bestandsnaam.c_str(), &w, &h, &n, 4);

	if(data)
	{
		width 	= w;
		height 	= h;
		kanalen	= 4;

		std::cout << "Plaatje '" << bestandsnaam << "' geladen en formaat: " << width << " X " << height << std::endl;

		return data;
	}

	return nullptr;
}