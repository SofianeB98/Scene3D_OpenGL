#include "Material.h"

Material::Material()
{
	for (int i = 0; i < 3; i++)
	{
		this->matAmbiant[i] = 1.0f;
		this->matDiffuse[i] = 1.0f;
		this->matSpecular[i] = 0.5f;
	}
	this->matShine = 5.f;
}

Material::~Material()
{
	delete[] this->matAmbiant;
	delete[] this->matDiffuse;
	delete[] this->matSpecular;
}