//Deze demo doet schapen en wolven met compute shaders
#include "../weergaveSchermVierkant.h"
#include "../subjecten/wolfSchaap.h"

int main()
{
	weergaveSchermVierkant scherm("Wolven & Schapen");

	//scherm.maakShader("bereken",	"shaders/computeDemo.comp");
	scherm.maakShader("geefWeer", 	"shaders/computeDemo.vert", "shaders/computeDemo.frag");

	Dieren dieren;

	while(!scherm.stopGewenst())
	{
		//scherm.doeRekenVerwerker("bereken", {
		scherm.bereidRenderVoor("geefWeer");
		//scherm.geefWeer();
		dieren.teken(true);
		dieren.teken(false);
		scherm.rondRenderAf();
	}
}