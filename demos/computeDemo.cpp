//Deze demo doet schapen en wolven met compute shaders
#include "../weergaveSchermVierkant.h"

int main()
{
	weergaveSchermVierkant scherm("Wolven & Schapen");

	scherm.maakShader("bereken",	"shaders/computeDemo.comp");
	scherm.maakShader("geefWeer", 	"shaders/computeDemo.vert", "shaders/computeDemo.frag");

	

	while(!scherm.stopGewenst())
	{
		scherm.doeRekenVerwerker("bereken", {
		scherm.bereidRenderVoor("geefWeer");
		scherm.geefWeer();
		scherm.rondRenderAf();
	}
}