//Deze demo doet schapen en wolven met compute shaders
#include <glad/glad.h>
#include "../weergaveSchermPerspectief.h"
#include "../subjecten/wolfSchaap.h"
#include <iostream>

int main()
{
	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);

	weergaveSchermPerspectief scherm("Wolven & Schapen")`;
	gladLoadGLLoader((GLADloadproc) glfwGetProcAddress);

	scherm.maakRekenShader(	"beweeg",		"shaders/computeDemoBeweeg.comp");
	scherm.maakShader(		"geefWeer", 	"shaders/computeDemo.vert", "shaders/computeDemo.frag");

	Dieren dieren(10, 200, 4.0);

	/*glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_BLEND);*/
    glClearColor(0.1f, 0.7f, 0.0f, 0.0f);
	glPointSize(10.0f);

	glEnable(GL_DEPTH_TEST);

	float rot = 0.0f;
	while(!scherm.stopGewenst())
	{
		scherm.herberekenProjectie();
		scherm.zetModelZicht(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.5f)), rot, glm::vec3(0.0f, 1.0f, 0.0f)));
		scherm.bereidRenderVoor("geefWeer");
		//scherm.geefWeer();
		dieren.teken(true);
		dieren.teken(false);
		scherm.rondRenderAf();

		rot+= 0.01f;

		dieren.beweeg(&scherm, false);
		dieren.pong();
	}
}