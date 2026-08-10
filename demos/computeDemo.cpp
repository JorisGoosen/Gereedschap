//Deze demo doet schapen en wolven met compute shaders
#include "../weergaveSchermPerspectief.h"
#include "../subjecten/wolfSchaap.h"
#include <iostream>

int main()
{
	weergaveSchermPerspectief scherm("Wolven & Schapen");

	scherm.maakRekenShader("beweeg", 	"shaders/computeDemoBeweeg.comp");
	scherm.maakShader("geefWeer", 		"shaders/computeDemo.vert", 	"shaders/computeDemo.frag");

	Dieren dieren(&scherm, 10, 200, 4.0);

	scherm.zetWeergaveKleur(0.1f, 0.7f, 0.0f, 0.0f);

	float rot = 0.0f;
	while(!scherm.stopGewenst())
	{
		scherm.herberekenProjectie();
		scherm.zetModelZicht(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f)), rot, glm::vec3(0.0f, 1.0f, 0.0f)));
		scherm.bereidWeergevenVoor("geefWeer");
		dieren.teken(true);
		dieren.teken(false);
		scherm.rondWeergevenAf();

		rot+= 0.01f;

		dieren.beweeg(false);
		dieren.pong();
	}
}