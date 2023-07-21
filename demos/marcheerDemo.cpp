//Dit zou een ray marching algo moeten laten zien ooit, we gaan het meemaken
#include "../weergaveSchermVierkant.h"

int main()
{
	weergaveSchermVierkant scherm("Marcheer Demo", 1024, 1024, true);
	scherm.maakShader("marcheerDemo", "shaders/marcheerDemo.vert", "shaders/marcheerDemo.frag");

	scherm.laadTextuurUitPng("plaatjes/landRuis.png", "landRuis", false, false, true);

	while(!scherm.stopGewenst())
	{
		scherm.bereidWeergevenVoor();
		scherm.bindTextuur("landRuis", glGetUniformLocation(scherm.huidigProgramma(), "landRuis"));
		scherm.geefWeer();
		scherm.rondWeergevenAf();
	}
}