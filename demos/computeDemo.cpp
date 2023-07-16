//Deze demo doet schapen en wolven met compute shaders
#include "../weergaveSchermPerspectief.h"
#include "../subjecten/wolfSchaap.h"

int main()
{
	weergaveSchermPerspectief scherm("Wolven & Schapen");

	//scherm.maakShader("bereken",	"shaders/computeDemo.comp");
	scherm.maakShader("geefWeer", 	"shaders/computeDemo.vert", "shaders/computeDemo.frag");

	Dieren dieren(1, 20, 2.0);

	/*glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);*/
    glClearColor(0.1f, 0.9f, 0.0f, 0.0f);
	glPointSize(10.0f);

	glEnable(GL_DEPTH_TEST);

	float rot = 0.0f;
	while(!scherm.stopGewenst())
	{
		scherm.herberekenProjectie();
		scherm.zetModelZicht(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f)), rot, glm::vec3(0.0f, 1.0f, 0.0f)));
		scherm.bereidRenderVoor("geefWeer");
		//scherm.geefWeer();
		dieren.teken(false);
	//	dieren.teken(false);
		scherm.rondRenderAf();

		rot+= 0.01f;
	}
}