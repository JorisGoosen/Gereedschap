#include "weergaveSchermVierkant.h"
#include <iostream>

using namespace glm;


weergaveSchermVierkant::weergaveSchermVierkant(std::string Naam, size_t W, size_t H) 
: weergaveScherm(Naam, W, H)
{}

/*void weergaveSchermVierkant::keyHandler(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(_glfwScherm, 1);
}*/

weergaveSchermVierkant::~weergaveSchermVierkant()
{
	delete _quadArray;
	delete _quadPos;
	delete _quadTex;
	
	_quadArray 	= nullptr;
	_quadPos 	= nullptr;
	_quadTex 	= nullptr;
}

void weergaveSchermVierkant::initQuad()
{
	if(_quadArray != NULL)
		throw std::runtime_error("InitQuad dubbel gedaan..\n");

	const int QuadArraySize = 32;
	//if(!SchrijfBeeldenWeg)	printf("InitQuad Gestart!\n");
	_quadArray = new wrgvOpslag();
	
	_quadPos = new wrgvOnderOpslag<float>(2, _quadArray, 0);
	_quadPos->ggvPuntErbij(vec2(-1.0f, -1.0f));
	_quadPos->ggvPuntErbij(vec2( 1.0f, -1.0f));
	_quadPos->ggvPuntErbij(vec2( 1.0f,  1.0f));
	_quadPos->ggvPuntErbij(vec2(-1.0f,  1.0f));
	
	_quadTex = new wrgvOnderOpslag<float>(2, _quadArray, 1);
	_quadTex->ggvPuntErbij(vec2(-1.0f, -1.0f));
	_quadTex->ggvPuntErbij(vec2( 1.0f, -1.0f));
	_quadTex->ggvPuntErbij(vec2( 1.0f,  1.0f));
	_quadTex->ggvPuntErbij(vec2(-1.0f,  1.0f));

	_quadPos->spoel();
	_quadTex->spoel();
}

void weergaveSchermVierkant::renderQuad()
{
	if(!_quadArray)
		initQuad();

	glDisable(GL_DEPTH_TEST);
	unsigned int Indices[] = {1, 0, 2, 3};

	_quadArray->BindVertexArray();	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, Indices);
	glErrorToConsole("weergaveSchermVierkant::renderQuad(): ");
}