#include "../weergaveSchermPerspectief.h"
#include "../geometrie/plylezer.h"

int main()
{
	weergaveSchermPerspectief scherm("Ply Demo");
	scherm.maakShader("plyDemo", "shaders/plyDemo.vert", "shaders/plyDemo.frag");

	glClearColor(0,0,0,0);

	plyLezer ply;
	ply.openPlyBestand("~/Photogram/BewerktePlys/beertjesuitsnede.ply");

	//float rot = 0.0f;
	//while(!scherm.stopGewenst())
	//{
	//	scherm.zetModelZicht(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)), rot, glm::vec3(0.0f, 1.0f, 0.0f)));
	//	scherm.bereidWeergevenVoor();
	//	ply.tekenJezelf();
	//	//scherm.geefVierkantWeer();
	//	scherm.rondWeergevenAf();
//
//
	//	rot += 0.001f;
	//}
}