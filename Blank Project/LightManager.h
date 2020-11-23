#pragma once
#include "Renderer.h"
# include "../nclgl/Light.h"


const int LIGHT_NUM = 32;

class LightManager
{
public:
	LightManager();
	~LightManager();
	void DrawLights();



protected:
	Light* pointLights; // Array of lighting data

};




