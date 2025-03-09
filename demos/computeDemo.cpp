//Deze demo doet schapen en wolven met compute shaders
#include "../weergaveSchermPerspectief.h"
#include "../subjecten/wolfSchaap.h"
#include <iostream>

int main()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);

	weergaveSchermPerspectief scherm("Wolven & Schapen");

	scherm.maakRekenShader(	"beweeg",		"shaders/computeDemoBeweeg.comp");
	scherm.maakShader(		"geefWeer", 	"shaders/computeDemo.vert", 	"shaders/computeDemo.frag");

	Dieren dieren(&scherm, 100, 2000, 4.0);

	/*glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);*/
    glClearColor(0.1f, 0.7f, 0.0f, 0.0f);
	glPointSize(10.0f);

	glEnable(GL_DEPTH_TEST);

	float rot = 0.0f;
	while(!scherm.stopGewenst())
	{
		glEnable(GL_DEPTH_TEST);
		scherm.herberekenProjectie();
		scherm.zetModelZicht(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -8.5f)), rot, glm::vec3(0.0f, 1.0f, 0.0f)));
		scherm.bereidWeergevenVoor("geefWeer");
		//scherm.geefVierkantWeer();
		dieren.teken(true);
		dieren.teken(false);
		scherm.rondWeergevenAf();

		//rot+= 0.0001f;
	
		scherm.laadOmgeving();
		glDisable(GL_DEPTH_TEST);
		dieren.beweeg(false);
		dieren.beweeg(true);
		dieren.pong();
	
	}
}