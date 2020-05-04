#include "RenderScherm.h"
#include <iostream>

using namespace glm;

RenderScherm::RenderScherm(std::string Naam, size_t W, size_t H) 
: AspectRatio(float(W) / float(H)), MijnNaam(Naam)
{
	std::cout << "RenderScherm created!" << std::endl;

    if (!glfwInit())
		throw std::runtime_error("Failed to intialize glfw");
        
    Scherm = glfwCreateWindow(W, H, MijnNaam.c_str(), nullptr, nullptr);

    if (!Scherm)
    {
        glfwTerminate();
        throw std::runtime_error("Failed to create window!");
    }

    glfwMakeContextCurrent(Scherm);

	GLenum err = glewInit();
	
	if (GLEW_OK != err)
	{
	  	std::cerr << "Error: " << glewGetErrorString(err) << std::endl;
		throw std::runtime_error("Failed to initalize glew...");
	}

	std::cout << "Status: Using GLEW " << glewGetString(	GLEW_VERSION) << std::endl;


	RecalculateProjection();

	glfwSwapInterval(1);
}

RenderScherm::~RenderScherm()
{
	delete QuadArray;
	delete QuadPos;
	delete QuadTex;
	
	QuadArray = nullptr;
	QuadPos = nullptr;
	QuadTex = nullptr;

   	glfwDestroyWindow(Scherm);
	glfwTerminate();
}

void RenderScherm::prepareForRender()
{
	glfwMakeContextCurrent(Scherm);

	int width, height;
	
	glfwGetFramebufferSize(Scherm, &width, &height);
	AspectRatio = width / (float) height;

	//std::cout << "Windowsize: " << width << "x" << height << std::endl;

	glViewport(0, 0, width, height);
}

void RenderScherm::finishRender()
{
	glFlush();
	glfwSwapBuffers(Scherm);
	glfwPollEvents();
}

void RenderScherm::InitQuad()
{
	if(QuadArray != NULL)
		throw std::runtime_error("InitQuad dubbel gedaan..\n");

	const int QuadArraySize = 32;
	//if(!SchrijfBeeldenWeg)	printf("InitQuad Gestart!\n");
	QuadArray = new ArrayOfStructOfArrays(QuadArraySize);
	
	QuadPos = new RenderSubBuffer(2, QuadArraySize, QuadArray, 0);
	QuadPos->AddDataPoint(vec2(-1.0f, -1.0f));
	QuadPos->AddDataPoint(vec2( 1.0f, -1.0f));
	QuadPos->AddDataPoint(vec2( 1.0f,  1.0f));
	QuadPos->AddDataPoint(vec2(-1.0f,  1.0f));
	
	QuadTex = new RenderSubBuffer(2, QuadArraySize, QuadArray, 1);
	QuadTex->AddDataPoint(vec2(-1.0f, -1.0f));
	QuadTex->AddDataPoint(vec2( 1.0f, -1.0f));
	QuadTex->AddDataPoint(vec2( 1.0f,  1.0f));
	QuadTex->AddDataPoint(vec2(-1.0f,  1.0f));

	QuadPos->Flush();
	QuadTex->Flush();

}

void RenderScherm::RenderQuad()
{
	if(!QuadArray)
		InitQuad();

	glDisable(GL_DEPTH_TEST);
	unsigned int Indices[] = {1, 0, 2, 3};

	QuadArray->BindVertexArray();	
	glDrawElements(GL_TRIANGLE_STRIP, 4, GL_UNSIGNED_INT, Indices);
	glErrorToConsole("RenderScherm::RenderQuad(): ");
}