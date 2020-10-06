#pragma once
#include "RenderScherm.h"

class RenderSchermQuad : public RenderScherm
{
public:
		RenderSchermQuad(std::string Naam = "RenderSchermQuad", size_t W = 1280, size_t H = 720);
		~RenderSchermQuad();

	void 		renderQuad();


	//void keyHandler(int key, int scancode, int action, int mods) override;

private:
	void		initQuad();
	
	wrgvOpslag 	*_quadArray		= nullptr;
	wrgvOnderOpslag<float>	*_quadPos		= nullptr,
							*_quadTex		= nullptr;
};