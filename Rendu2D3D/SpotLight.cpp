#include "SpotLight.h"

SpotLight::SpotLight()
{
	for (int i = 0; i < 3; i++)
	{
		this->lightDirection[i] = 1.0f;

		this->lightPosition[i] = 2.0f;
		
		this->lightDiffuse[i] = 1.0f;
		this->lightSpecular[i] = 0.8f;
		this->lightAmbient[i] = 0.2f;
	}

	this->cutoff = 30.0f; //doit etre en radiant
	this->outerCutoff = 35.0f;
}

SpotLight::~SpotLight()
{
	delete[] this->lightDiffuse;
	delete[] this->lightDirection;
	delete[] this->lightAmbient;
	delete[] this->lightSpecular;
}

void SpotLight::setDirection(float x, float y, float z)
{
	this->lightDirection[0] = x;
	this->lightDirection[1] = y;
	this->lightDirection[2] = z;
}

void SpotLight::setPosition(float x, float y, float z)
{
	this->lightPosition[0] = x;
	this->lightPosition[1] = y;
	this->lightPosition[2] = z;
}

void SpotLight::setDiffuse(float x, float y, float z)
{
	this->lightDiffuse[0] = x;
	this->lightDiffuse[1] = y;
	this->lightDiffuse[2] = z;
}

void SpotLight::setSpecular(float x, float y, float z)
{
	this->lightSpecular[0] = x;
	this->lightSpecular[1] = y;
	this->lightSpecular[2] = z;
}

void SpotLight::setAmbiant(float x, float y, float z)
{
	this->lightAmbient[0] = x;
	this->lightAmbient[1] = y;
	this->lightAmbient[2] = z;
}