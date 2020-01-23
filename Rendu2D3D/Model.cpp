#include "Model.h"
#include <tiny_obj_loader.h>
#include <stdexcept>
#include <iostream>

Model::Model()
{
	
}

Model::Model(char* modelPath, char* vShaderName, char* fShaderName, char* materialPath)
{
	//Assignation des path et du shader name
	this->modelPath = modelPath;
	this->materialPath = materialPath;
	this->vShaderName = vShaderName;
	this->fShaderName = fShaderName;
		
	//Initialisation de la matrice du model
	this->matriceTranslation = new Mat4();
	this->matriceRot = new Mat4();
	this->matriceScale = new Mat4();
	
	this->worldMatrix = new Mat4();
	
	this->matriceTranslation->Translate(0.f, 0.f, 0.f);
	this->matriceScale->Scale(1.0f);
	
	this->worldMatrix = this->worldMatrix->Multiply(*this->matriceScale, *this->matriceRot);
	this->worldMatrix = this->worldMatrix->Multiply(*this->worldMatrix, *this->matriceTranslation);
}

void Model::CheckExistingVertex(Vertex3D& v)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if (v.equalNormal(vertices[i]) && v.equalPosition(vertices[i]) && v.equalTexcoord(vertices[i]))
		{
			indices.push_back(i);
			return;
		}
	}
	vertices.push_back(v);
	indices.push_back(vertices.size() - 1);
}

void Model::LoadModel()
{
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;

	std::string warn;
	std::string err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, modelPath, materialPath))
	{
		throw std::runtime_error(warn + err);
	}

	std::cout << "# of materials : " << materials.size() << std::endl;

	for (const auto& shape : shapes)
	{
		for (const auto& index : shape.mesh.indices)
		{
			Vertex3D vertex = {};

			vertex.x = attrib.vertices[3 * index.vertex_index + 0];
			vertex.y = attrib.vertices[3 * index.vertex_index + 1];
			vertex.z = attrib.vertices[3 * index.vertex_index + 2];

			vertex.nx = attrib.normals[3 * index.normal_index + 0];
			vertex.ny = attrib.normals[3 * index.normal_index + 1];
			vertex.nz = attrib.normals[3 * index.normal_index + 2];

			vertex.u = attrib.texcoords[2 * index.texcoord_index + 0];
			vertex.v = 1 - attrib.texcoords[2 * index.texcoord_index + 1];

			vertex.r = 1.f;
			vertex.g = 1.f;
			vertex.b = 1.f;

			CheckExistingVertex(vertex);
		}
	}

	if (materials.size() > 0)
	{
		mat->matAmbiant[0] = materials[0].ambient[0];
		mat->matAmbiant[1] = materials[0].ambient[1];
		mat->matAmbiant[2] = materials[0].ambient[2];

		mat->matDiffuse[0] = materials[0].diffuse[0];
		mat->matDiffuse[1] = materials[0].diffuse[1];
		mat->matDiffuse[2] = materials[0].diffuse[2];

		mat->matSpecular[0] = materials[0].specular[0];
		mat->matSpecular[1] = materials[0].specular[1];
		mat->matSpecular[2] = materials[0].specular[2];

		mat->matShine = materials[0].shininess;
	}
}

void Model::CreateAndLoadShader()
{
	this->g_BasicShader.LoadVertexShader((const char*)this->vShaderName);
	this->g_BasicShader.LoadFragmentShader((const char*)this->fShaderName);
	this->g_BasicShader.Create();
	this->basicProgram = this->g_BasicShader.GetProgram();
}
