#include "LightManager.h"


LightManager::LightManager()
{

	pointLights = new Light[LIGHT_NUM];

	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		Light& l = pointLights[i];
		l.SetPosition(Vector3(rand() % 300,
			150.0f,
			rand() % 300));

		l.SetColour(Vector4(0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			0.5f + (float)(rand() / (float)RAND_MAX),
			1));
		l.SetRadius(250.0f + (rand() % 250));
	}
}

LightManager::~LightManager()
{
	delete[] pointLights;
}

void LightManager::DrawLights()
{
	for (int i = 0; i < LIGHT_NUM; ++i)
	{
		Light& l = pointLights[i];
		SetShaderLight(l);
		sphere->Draw();
	}
}