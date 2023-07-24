//Dit zou een ray marching algo moeten laten zien ooit, we gaan het meemaken
#include "../weergaveSchermVierkant.h"

int main()
{
	weergaveSchermVierkant scherm("Marcheer Demo", 1024, 1024, false);
	scherm.maakShader("marcheerDemo", "shaders/marcheerDemo.vert", "shaders/marcheerDemo.frag");

	scherm.laadTextuurUitPng("plaatjes/handLand.png", "handLand", true, true, false);

	while(!scherm.stopGewenst())
	{
		scherm.bereidWeergevenVoor();
		scherm.bindTextuur("handLand", glGetUniformLocation(scherm.huidigProgramma(), "landRuis"));
		scherm.geefWeer();
		scherm.rondWeergevenAf();
	}
}