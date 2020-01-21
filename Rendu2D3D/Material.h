#pragma once

struct Material
{
	float matAmbiant[3];
	float matDiffuse[3];
	float matSpecular[3];
	float matShine;

	Material();
	~Material(); //destructeur
};
