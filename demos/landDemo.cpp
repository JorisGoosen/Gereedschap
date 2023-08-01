//Dit zou een landschap dmv driehoeken moeten laten zien
#include "../weergaveSchermPerspectief.h"
#include "../geometrie/vierkantRooster.h"
#include <iostream>

int main(int argc, const char * argv[])
{
	bool toonHetLand = argc > 1 && argv[1] == std::string("toonHetLand");

	weergaveSchermPerspectief scherm("Marcheer Demo", 1024, 1024, false);
	
	scherm.maakShader("landDemo", 	"shaders/landDemo.vert", 	"shaders/landDemo.frag");
	
	glm::uvec2 landGrootte = scherm.laadTextuurUitPng("plaatjes/handLand.png", "handLand", false, false, true);

	vierkantRooster landRooster(landGrootte.x, landGrootte.y);
	
	float rot = 0.;
	while(!scherm.stopGewenst())
	{
		scherm.herberekenProjectie();
		scherm.zetModelZicht(glm::rotate(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -0.5f, -1.5f)), rot, glm::normalize(glm::vec3(0.0f, 1.0f, 0.0f))));
		scherm.bereidWeergevenVoor("landDemo");
		scherm.bindTextuur("handLand", 0);
		glUniform1i(glGetUniformLocation(scherm.huidigProgramma(), "handLand"), 0);
		landRooster.tekenJezelf();
		scherm.rondWeergevenAf();

		rot += 0.007f;
	}
}