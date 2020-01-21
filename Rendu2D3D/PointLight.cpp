#include "PointLight.h"

PointLight::PointLight()
{
	for (int i = 0; i < 3; i++)
	{
		this->lightPosition[i] = 1.0f;

		this->lightDiffuse[i] = 0.5f;
		this->lightSpecular[i] = 0.5f;
		this->lightAmbient[i] = 0.3f;
	}
	
	this->constant = 1.0f;
	this->linear = 0.09f;
	this->quadratic = 0.032f;
}

PointLight::~PointLight()
{
	delete[] this->lightDiffuse;
	delete[] this->lightPosition;
	delete[] this->lightAmbient;
	delete[] this->lightSpecular;
}

void PointLight::setPosition(float x, float y, float z)
{
	this->lightPosition[0] = x;
	this->lightPosition[1] = y;
	this->lightPosition[2] = z;
}

void PointLight::setDiffuse(float x, float y, float z)
{
	this->lightDiffuse[0] = x;
	this->lightDiffuse[1] = y;
	this->lightDiffuse[2] = z;
}

void PointLight::setSpecular(float x, float y, float z)
{
	this->lightSpecular[0] = x;
	this->lightSpecular[1] = y;
	this->lightSpecular[2] = z;
}

void PointLight::setAmbiant(float x, float y, float z)
{
	this->lightAmbient[0] = x;
	this->lightAmbient[1] = y;
	this->lightAmbient[2] = z;
}