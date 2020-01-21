#pragma once

struct PointLight
{
	float lightPosition[3];

	float lightDiffuse[3];
	float lightSpecular[3];
	float lightAmbient[3];

	//Attenuation
	float constant;
	float linear;
	float quadratic;

	PointLight();
	~PointLight(); //destructeur

	void setPosition(float x, float y, float z);
	void setDiffuse(float x, float y, float z);
	void setSpecular(float x, float y, float z);
	void setAmbiant(float x, float y, float z);
};
