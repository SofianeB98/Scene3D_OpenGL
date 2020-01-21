#pragma once

struct SpotLight
{
	float lightPosition[3];
	float lightDirection[3];
	
	float lightDiffuse[3];
	float lightSpecular[3];
	float lightAmbient[3];

	//Attenuation
	float cutoff;
	float outerCutoff;
	
	SpotLight();
	~SpotLight(); //destructeur

	void setPosition(float x, float y, float z);
	void setDirection(float x, float y, float z);

	void setDiffuse(float x, float y, float z);
	void setSpecular(float x, float y, float z);
	void setAmbiant(float x, float y, float z);
};
