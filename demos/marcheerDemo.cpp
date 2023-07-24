//Dit zou een ray marching algo moeten laten zien ooit, we gaan het meemaken
#include "../weergaveSchermVierkant.h"
#include "../nepScherm.h"
int main()
{
	weergaveSchermVierkant scherm("Marcheer Demo", 1024, 1024, false);
	scherm.maakShader("marcheerDemo", "shaders/marcheerDemo.vert", "shaders/marcheerDemo.frag");
	scherm.maakShader("bewerkHetLand", "shaders/bewerkHetLand.vert", "shaders/bewerkHetLand.frag");

	scherm.laadTextuurUitPng("plaatjes/handLand.png", "handLand", true, true, false);
	scherm.laadTextuurUitPng("plaatjes/handLand.png", "handLandTwee", true, true, false);

	nepScherm nepperd(&scherm, "handLandTwee");

	nepperd.bereidWeergevenVoor("bewerkHetLand");
	scherm.geefWeer();
	nepperd.rondWeergevenAf();

	while(!scherm.stopGewenst())
	{
		scherm.bereidWeergevenVoor("marcheerDemo");
		scherm.bindTextuur("handLand", glGetUniformLocation(scherm.huidigProgramma(), "landRuis"));
		scherm.geefWeer();
		scherm.rondWeergevenAf();
	}
}