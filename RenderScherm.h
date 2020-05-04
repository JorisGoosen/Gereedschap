#pragma once
#include <GL/glew.h>
#include <GL/gl.h>
#include <string>
#include <GLFW/glfw3.h>
#define GLM_FORCE_RADIANS
#define GLM_FORCE_SWIZZLE 
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "RenderSubBuffer.h"	

	
const long double 	PI  = 3.141592653589793238L;
const float 		FPI = 3.141592653589793238f;

class RenderScherm
{
public:
		RenderScherm(std::string Naam = "RenderScherm", size_t W = 1280, size_t H = 720);
		~RenderScherm();

	void		RecalculateProjection()	{ Projection = glm::perspective(FovYScale, AspectRatio, NearDistance, FarDistance); };
	void		RecalculateModelView()	{ RecalculateModelView(true); }
	void		RecalculateModelView(bool IncorporateTranslation);

	glm::mat4	GetProjection()			{ return Projection; }
	glm::mat4	GetModelView()			{ return ModelView; }

	void		prepareForRender();
	void		finishRender();

	void 		RenderQuad();

	bool		closeRequested() { return glfwWindowShouldClose(Scherm); }

	float		FovYScale			= FPI * 0.25f,
				AspectRatio			= 16.0f / 9.0f,
				NearDistance		= 0.001f,
				FarDistance			= 100.0f;

private:
	void		InitQuad();
	
	glm::mat4				Projection, 
							ModelView;

 	GLFWwindow 		* 		Scherm = nullptr;

	std::string				MijnNaam;
	
	ArrayOfStructOfArrays 	*QuadArray		= NULL;
	RenderSubBuffer			*QuadPos		= NULL,
							*QuadTex		= NULL;

};