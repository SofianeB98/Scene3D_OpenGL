#include "DirectionalLight.h"

DirectionalLight::DirectionalLight()
{
	for (int i = 0; i < 3; i++)
	{
		this->lightDirection[i] = 2.0f;
		
		this->lightDiffuse[i] = 0.5f;
		this->lightSpecular[i] = 0.4f;
		this->lightAmbient[i] = 0.3f;
	}
}

DirectionalLight::~DirectionalLight()
{
	delete[] this->lightDiffuse;
	delete[] this->lightDirection;
	delete[] this->lightAmbient;
	delete[] this->lightSpecular;
}

void DirectionalLight::setDirection(float x, float y, float z)
{
	this->lightDirection[0] = x;
	this->lightDirection[1] = y;
	this->lightDirection[2] = z;
}

void DirectionalLight::setDiffuse(float x, float y, float z)
{
	this->lightDiffuse[0] = x;
	this->lightDiffuse[1] = y;
	this->lightDiffuse[2] = z;
}

void DirectionalLight::setSpecular(float x, float y, float z)
{
	this->lightSpecular[0] = x;
	this->lightSpecular[1] = y;
	this->lightSpecular[2] = z;
}

void DirectionalLight::setAmbiant(float x, float y, float z)
{
	this->lightAmbient[0] = x;
	this->lightAmbient[1] = y;
	this->lightAmbient[2] = z;
}