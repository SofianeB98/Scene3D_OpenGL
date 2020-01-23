#include "Material.h"
#include <iostream>

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

Material::Material(const Material& mat)
{
	for (int i = 0; i < 3; i++)
	{
		this->matAmbiant[i] = mat.matAmbiant[i];
		this->matDiffuse[i] = mat.matDiffuse[i];
		this->matSpecular[i] = mat.matSpecular[i];
	}
	this->matShine = mat.matShine;
}

Material& Material::operator=(Material mat)
{
	this->swap(mat);

	return *this;
}

void Material::swap(Material& mat)
{
	std::swap(this->matAmbiant,mat.matAmbiant);
	std::swap(this->matDiffuse, mat.matDiffuse);
	std::swap(this->matSpecular, mat.matSpecular);
	std::swap(this->matShine, mat.matShine);
}

Material::~Material()
{
	delete[] this->matAmbiant;
	delete[] this->matDiffuse;
	delete[] this->matSpecular;
}