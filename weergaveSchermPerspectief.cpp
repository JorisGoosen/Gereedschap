#include "weergaveSchermPerspectief.h"
#include <iostream>

using namespace glm;


weergaveSchermPerspectief::weergaveSchermPerspectief(std::string Naam, size_t W, size_t H, size_t samples) 
: weergaveSchermVierkant(Naam, W, H, samples)
{
	herberekenProjectie();
	herberekenModelZicht();
}

void weergaveSchermPerspectief::toetsVerwerker(int key, int scancode, int action, int mods)
{
	weergaveScherm::toetsVerwerker(key, scancode, action, mods); //We hoeven niks te controleren want als die iets opvangt sluit het programma

	const float stap = 0.1, tol = 0.2;

	if(action == GLFW_PRESS || action == GLFW_REPEAT)
		switch(key)
		{
		case GLFW_KEY_W:		_verplaatsing.z += stap;	break;
		case GLFW_KEY_S:		_verplaatsing.z -= stap;	break;
		case GLFW_KEY_A:		_verplaatsing.x -= stap;	break;
		case GLFW_KEY_D:		_verplaatsing.x += stap;	break;
		case GLFW_KEY_Q:		_verplaatsing.y += stap;	break;
		case GLFW_KEY_E:		_verplaatsing.y -= stap;	break;

		case GLFW_KEY_UP:		_verdraaiing.y	-= tol;		break;
		case GLFW_KEY_DOWN:		_verdraaiing.y	+= tol;		break;
		case GLFW_KEY_LEFT:		_verdraaiing.x	-= tol;		break;
		case GLFW_KEY_RIGHT:	_verdraaiing.x	+= tol;		break;
		}

	herberekenModelZicht();
}

void weergaveSchermPerspectief::herberekenModelZicht()
{
	zetModelZicht(
		glm::rotate(
			glm::rotate(
				glm::translate(
					glm::mat4(1.0f),
					_verplaatsing
				),
				_verdraaiing.y,
				glm::vec3(1.0f, 0.0f, 0.0f)
			),
			_verdraaiing.x,
			glm::vec3(0.0f, 1.0f, 0.0f)
		)
	);
}

weergaveSchermPerspectief::~weergaveSchermPerspectief()
{
}

void weergaveSchermPerspectief::zetModelZicht(glm::mat4 modelView)
{ 
	_modelZicht  = modelView;
	_transInvMV = glm::transpose(glm::inverse(_modelZicht));
}

void weergaveSchermPerspectief::extraVoorbereidingen(GLuint programma)
{
	glEnable(GL_DEPTH_TEST);

	herberekenProjectie();	

	glUniformMatrix4fv(glGetUniformLocation(programma, "projectie"), 1, GL_FALSE, glm::value_ptr(_projectie));
	glErrorToConsole("weergaveSchermPerspectief::extraVoorbereidingen() -> projectie");

	glUniformMatrix4fv(glGetUniformLocation(programma, "modelView"), 1, GL_FALSE, glm::value_ptr(_modelZicht));
	glErrorToConsole("weergaveSchermPerspectief::extraVoorbereidingen() -> modelView");

	glUniformMatrix4fv(glGetUniformLocation(programma, "transInvMV"), 1, GL_FALSE, glm::value_ptr(_transInvMV));
	glErrorToConsole("weergaveSchermPerspectief::extraVoorbereidingen() -> transInvMV");


	
}
