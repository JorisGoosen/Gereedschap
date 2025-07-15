#include "../weergaveSchermPerspectief.h"
#include "../geometrie/plylezer.h"
#include <iostream>

int main()
{
	weergaveSchermPerspectief scherm("Ply Demo");
	scherm.maakShader("plyDemo", "shaders/plyDemo.vert", "shaders/plyDemo.frag");

	glClearColor(0,0,0,0);

	plyLezer ply;

	if(!ply.openPlyBestand("/home/falafel/Photogram/BewerktePlys/beertjesuitsnede.ply"))
		std::cout << "Inlezen bestand mislukt..." << std::endl;

	float rot = 0.0f;
	while(!scherm.stopGewenst())
	{
		scherm.zetModelZicht(
			glm::rotate(
				glm::rotate(
					glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -4.0f)), 
					180.f,
					glm::vec3(1.0f, 0.0f, 0.0f)
					), 
					rot, glm::vec3(0.0f, 1.0f, 0.0f)));
		scherm.bereidWeergevenVoor();
		ply.tekenJezelf();
		//scherm.geefVierkantWeer();
		scherm.rondWeergevenAf();
	
	
		rot += 0.001f;
	}
}