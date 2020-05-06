#include "RenderSchermQuad.h"
#include <iostream>

using namespace glm;


RenderSchermQuad::RenderSchermQuad(std::string Naam, size_t W, size_t H) 
: RenderScherm(Naam, W, H)
{}

/*void RenderSchermQuad::keyHandler(int key, int scancode, int action, int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(_glfwScherm, 1);
}*/

RenderSchermQuad::~RenderSchermQuad()
{
	delete _quadArray;
	delete _quadPos;
	delete _quadTex;
	
	_quadArray 	= nullptr;
	_quadPos 	= nullptr;
	_quadTex 	= nullptr;
}

void RenderSchermQuad::initQuad()
{
	if(_quadArray != NULL)
		throw std::runtime_error("InitQuad dubbel gedaan..\n");

	const int QuadArraySize = 32;
	//if(!SchrijfBeeldenWeg)	printf("InitQuad Gestart!\n");
	_quadArray = new ArrayOfStructOfArrays(QuadArraySize);
	
	_quadPos = new RenderSubBuffer<float>(2, QuadArraySize, _quadArray, 0);
	_quadPos->AddDataPoint(vec2(-1.0f, -1.0f));
	_quadPos->AddDataPoint(vec2( 1.0f, -1.0f));
	_quadPos->AddDataPoint(vec2( 1.0f,  1.0f));
	_quadPos->AddDataPoint(vec2(-1.0f,  1.0f));
	
	_quadTex = new RenderSubBuffer<float>(2, QuadArraySize, _quadArray, 1);
	_quadTex->AddDataPoint(vec2(-1.0f, -1.0f));
	_quadTex->AddDataPoint(vec2( 1.0f, -1.0f));
	_quadTex->AddDataPoint(vec2( 1.0f,  1.0f));
	_quadTex->AddDataPoint(vec2(-1.0f,  1.0f));

	_quadPos->Flush();
	_quadTex->Flush();
}

void RenderSchermQuad::renderQuad()
{
	if(!_quadArray)
		initQuad();

	glDisable(GL_DEPTH_TEST);
	unsigned int Indices[] = {1, 0, 2, 3};

	_quadArray->BindVertexArray();	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, Indices);
	glErrorToConsole("RenderSchermQuad::renderQuad(): ");
}