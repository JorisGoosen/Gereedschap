#include "weergaveSchermPerspectief.h"
#include <iostream>

using namespace glm;


 weergaveSchermPerspectief::weergaveSchermPerspectief(std::string Naam, size_t W, size_t H, size_t samples, bool hoofdloos) 
: weergaveSchermVierkant(Naam, W, H, false, hoofdloos)
{
	herberekenProjectie();
	herberekenModelZicht();
}

void weergaveSchermPerspectief::toetsVerwerker(int key, int scancode, int action, int mods)
{
	weergaveScherm::toetsVerwerker(key, scancode, action, mods); //We hoeven niks te controleren want als die iets opvangt sluit het programma

	const float stap = 0.1, tol = 0.2;
	bool cameraAangeraakt = false;

	if(action == GLFW_PRESS || action == GLFW_REPEAT)
		switch(key)
		{
		case GLFW_KEY_W:		_verplaatsing.z += stap;	cameraAangeraakt = true;	break;
		case GLFW_KEY_S:		_verplaatsing.z -= stap;	cameraAangeraakt = true;	break;
		case GLFW_KEY_A:		_verplaatsing.x -= stap;	cameraAangeraakt = true;	break;
		case GLFW_KEY_D:		_verplaatsing.x += stap;	cameraAangeraakt = true;	break;
		case GLFW_KEY_Q:		_verplaatsing.y += stap;	cameraAangeraakt = true;	break;
		case GLFW_KEY_E:		_verplaatsing.y -= stap;	cameraAangeraakt = true;	break;

		case GLFW_KEY_UP:		_verdraaiing.y	-= tol;		cameraAangeraakt = true;	break;
		case GLFW_KEY_DOWN:		_verdraaiing.y	+= tol;		cameraAangeraakt = true;	break;
		case GLFW_KEY_LEFT:		_verdraaiing.x	-= tol;		cameraAangeraakt = true;	break;
		case GLFW_KEY_RIGHT:	_verdraaiing.x	+= tol;		cameraAangeraakt = true;	break;
		}

	if(cameraAangeraakt)
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

void weergaveSchermPerspectief::extraVoorbereidingen(WGPURenderPipeline programma)
{
	herberekenProjectie();	

	//Projectie, modelZicht en transInvMV gaan als drie mat4's naar de matrix-buffer.
	//GLM slaat mat4 op in kolom-voorkeur, net als een WGSL uniform mat4.
	glm::mat4 matrices[3] = { _projectie, _modelZicht, _transInvMV };

	wgpuQueueWriteBuffer(_wgpRij, _matrixBuffer, 0, glm::value_ptr(matrices[0]), sizeof(matrices));

	wgpFoutControle("weergaveSchermPerspectief::extraVoorbereidingen(): ");

	(void)programma; //de matrices staan in de gedeelde matrix-buffer
}