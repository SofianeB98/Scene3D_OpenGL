#pragma once

struct DirectionalLight
{
	float lightDirection[3];
	
	float lightDiffuse[3];
	float lightSpecular[3];
	float lightAmbient[3];
	
	DirectionalLight();
	~DirectionalLight(); //destructeur

	void setDirection(float x, float y, float z);
	void setDiffuse(float x, float y, float z);
	void setSpecular(float x, float y, float z);
	void setAmbiant(float x, float y, float z);
};
