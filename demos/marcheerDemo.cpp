//Dit zou een ray marching algo moeten laten zien ooit, we gaan het meemaken
#include "../weergaveSchermVierkant.h"
#include "../nepScherm.h"
#include <iostream>

int main(int argc, const char * argv[])
{
	bool toonHetLand = argc > 1 && argv[1] == std::string("toonHetLand");

	weergaveSchermVierkant scherm("Marcheer Demo", 1024, 1024, false);
	
	scherm.maakShader("bewerkHetLand", 	"shaders/bewerkHetLand.vert", 	"shaders/bewerkHetLand.frag");
	scherm.maakShader("toonHetLand", 	"shaders/toonHetLand.vert", 	"shaders/toonHetLand.frag"	);
	scherm.maakShader("marcheerDemo", 	"shaders/marcheerDemo.vert", 	"shaders/marcheerDemo.frag"	);

	glm::uvec2 textuurGrootte = scherm.laadTextuurUitPng("plaatjes/handLand.png", "handLand", false, false, false);
	//scherm.maakTextuur("handLandTwee", textuurGrootte.x, textuurGrootte.y, false, false, false);
	scherm.laadTextuurUitPng("plaatjes/handLand.png", "handLandTwee", false, false, false);

	nepScherm nepperd(&scherm, "handLandTwee");

	scherm.initVierkant();
	nepperd.bereidWeergevenVoor("bewerkHetLand");
	scherm.bindTextuur("handLand", glGetUniformLocation(scherm.huidigProgramma(), "landRuis"));
	scherm.geefWeer();
	nepperd.rondWeergevenAf();
	
	glMemoryBarrier(GL_FRAMEBUFFER_BARRIER_BIT | GL_TEXTURE_UPDATE_BARRIER_BIT);

	
	if(glCheckFramebufferStatus(GL_DRAW_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "Er is een fout opgetreden tijdens het bewerken van het land en dat leidde iedereen nogal af. Het programma wordt beeindigd." << std::endl;
		exit(1);
	}

	while(!scherm.stopGewenst())
	{
		if(toonHetLand)		scherm.bereidWeergevenVoor("toonHetLand"); 
		else				scherm.bereidWeergevenVoor("marcheerDemo");

		scherm.bindTextuur("handLandTwee", glGetUniformLocation(scherm.huidigProgramma(), "landTwee"));
		scherm.geefWeer();
		scherm.rondWeergevenAf();
	}
}