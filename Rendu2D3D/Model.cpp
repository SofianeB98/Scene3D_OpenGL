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

	this->matriceTranslation->Translate(05.f, 0.f, 0.f);
	this->matriceScale->Scale(1.0f);

	this->worldMatrix = this->worldMatrix->Multiply(*this->matriceScale, *this->matriceRot);
	this->worldMatrix = this->worldMatrix->Multiply(*this->worldMatrix, *this->matriceTranslation);
}

Model::Model(const Model& cop)
{
	std::cout << "copy" << std::endl;
	this->matriceTranslation = new Mat4();
	this->matriceRot = new Mat4();
	this->matriceScale = new Mat4();
	this->worldMatrix = new Mat4();

	this->modelPath = cop.modelPath;
	this->materialPath = cop.materialPath;

	this->vertices.empty();

	//for(int i = 0; i < cop.vertices.size(); i++)
	//{
	//	this->vertices.push_back(cop.vertices[i]);
	//}

	this->indices.empty();

	//for (int i = 0; i < cop.indices.size(); i++)
	//{
	//	this->indices.push_back(cop.indices[i]);
	//}

	if(cop.mat != NULL)
		this->mat = new Material(*cop.mat);

	this->g_BasicShader = cop.g_BasicShader;
	this->vShaderName = cop.vShaderName;
	this->fShaderName = cop.fShaderName;
	this->basicProgram = cop.basicProgram;

	this->VBO = cop.VBO;
	this->IBO = cop.IBO;
	this->VAO = cop.VAO;

	if(cop.mat != NULL)
		this->CreateAndLoadShader();
	
	//this->matriceRot = cop.matriceRot;
	//this->matriceScale = cop.matriceScale;
	//this->matriceTranslation = cop.matriceTranslation;
	//this->worldMatrix = cop.worldMatrix;
	//
	//this->loc_position= cop.loc_position;
	//this->normals= cop.normals;
	//this->texCoords= cop.texCoords;
	//this->loc_color= cop.loc_color;

	//this->matriceProjLocation= cop.matriceProjLocation;
	//this->matriceWorld= cop.matriceWorld;
	//this->viewLocation= cop.viewLocation;

	//this->lightPositionLoc= cop.lightPositionLoc;
	//this->lightColorLoc= cop.lightColorLoc;
	//this->lightSpecLoc= cop.lightSpecLoc;
	//this->ambiantStrLoc= cop.ambiantStrLoc;
	//this->attenuationConstant= cop.attenuationConstant;
	//this->attenuationLinear= cop.attenuationLinear;
	//this->attenuationQuadratic= cop.attenuationQuadratic;

	//this->dlightPositionLoc= cop.dlightPositionLoc;
	//this->dlightColorLoc= cop.dlightColorLoc;
	//this->dlightSpecLoc= cop.dlightSpecLoc;
	//this->dambiantStrLoc= cop.dambiantStrLoc;

	//this->slightDirLoc= cop.slightDirLoc;
	//this->slightPositionLoc= cop.slightPositionLoc;
	//this->slightColorLoc= cop.slightColorLoc;
	//this->slightSpecLoc= cop.slightSpecLoc;
	//this->sambiantStrLoc= cop.sambiantStrLoc;
	//this->scutoffLoc= cop.scutoffLoc;
	//this->soutCutoffLoc= cop.soutCutoffLoc;

	//this->skyColorLoc= cop.skyColorLoc;
	//this->groundColorLoc= cop.groundColorLoc;

	//this->viewPosLoc= cop.viewPosLoc;

	//this->matDiffLoc= cop.matDiffLoc;
	//this->matAmbLoc= cop.matAmbLoc;
	//this->matSpecLoc= cop.matSpecLoc;
	//this->matShineLoc= cop.matShineLoc;
}

Model& Model::operator=(Model mod)
{
	this->swap(mod);
	
	return *this;
}

void Model::swap(Model& mod)
{
	std::swap(this->modelPath ,mod.modelPath);
	std::swap(this->materialPath ,mod.materialPath);
	std::swap(this->vShaderName ,mod.vShaderName);
	std::swap(this->fShaderName ,mod.fShaderName);

	std::swap(this->vertices ,mod.vertices);
	std::swap(this->indices ,mod.indices);

	std::swap(this->g_BasicShader ,mod.g_BasicShader);
	std::swap(this->basicProgram ,mod.basicProgram);

	std::swap(this->IBO ,mod.IBO);
	std::swap(this->VAO ,mod.VAO);
	std::swap(this->VBO ,mod.VBO);

	std::swap(this->loc_position ,mod.loc_position);
	std::swap(this->normals ,mod.normals);
	std::swap(this->texCoords ,mod.texCoords);
	std::swap(this->loc_color ,mod.loc_color);

	std::swap(this->matriceProjLocation ,mod.matriceProjLocation);
	std::swap(this->matriceWorld ,mod.matriceWorld);
	std::swap(this->viewLocation ,mod.viewLocation);

	std::swap(this->lightPositionLoc ,mod.lightPositionLoc);
	std::swap(this->lightColorLoc ,mod.lightColorLoc);
	std::swap(this->lightSpecLoc ,mod.lightSpecLoc);
	std::swap(this->ambiantStrLoc ,mod.ambiantStrLoc);
	std::swap(this->attenuationConstant ,mod.attenuationConstant);
	std::swap(this->attenuationLinear ,mod.attenuationLinear);
	std::swap(this->attenuationQuadratic ,mod.attenuationQuadratic);

	std::swap(this->dlightPositionLoc ,mod.dlightPositionLoc);
	std::swap(this->dlightColorLoc ,mod.dlightColorLoc);
	std::swap(this->dlightSpecLoc ,mod.dlightSpecLoc);
	std::swap(this->dambiantStrLoc ,mod.dambiantStrLoc);

	std::swap(this->slightDirLoc ,mod.slightDirLoc);
	std::swap(this->slightPositionLoc ,mod.slightPositionLoc);
	std::swap(this->slightColorLoc ,mod.slightColorLoc);
	std::swap(this->slightSpecLoc ,mod.slightSpecLoc);
	std::swap(this->sambiantStrLoc ,mod.sambiantStrLoc);
	std::swap(this->scutoffLoc ,mod.scutoffLoc);
	std::swap(this->soutCutoffLoc ,mod.soutCutoffLoc);

	std::swap(this->skyColorLoc ,mod.skyColorLoc);
	std::swap(this->groundColorLoc ,mod.groundColorLoc);

	std::swap(this->viewPosLoc ,mod.viewPosLoc);

	std::swap(this->matDiffLoc ,mod.matDiffLoc);
	std::swap(this->matAmbLoc ,mod.matAmbLoc);
	std::swap(this->matSpecLoc ,mod.matSpecLoc);
	std::swap(this->matShineLoc ,mod.matShineLoc);

	//mat4 et material a faire aussi
	std::swap(this->mat ,mod.mat);
	std::swap(this->matriceRot ,mod.matriceRot);
	std::swap(this->matriceScale ,mod.matriceScale);
	std::swap(this->matriceTranslation ,mod.matriceTranslation);
	std::swap(this->worldMatrix ,mod.worldMatrix);
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
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, (const char*)modelPath, (const char*)materialPath))
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

	mat = new Material();

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

	//----Pos normal etc

	loc_position = glGetAttribLocation(basicProgram, "a_position");
	normals = glGetAttribLocation(basicProgram, "a_normals");
	texCoords = glGetAttribLocation(basicProgram, "a_texcoords");
	loc_color = glGetAttribLocation(basicProgram, "a_color");

	//----Matrice

	matriceProjLocation = glGetUniformLocation(basicProgram, "u_matriceProjection");
	matriceWorld = glGetUniformLocation(basicProgram, "u_worldMatrix");
	viewLocation = glGetUniformLocation(basicProgram, "u_view");

	glUniformMatrix4fv(matriceWorld, 1, false, worldMatrix->matrice);

	//----Lights

	lightPositionLoc = glGetUniformLocation(basicProgram, "u_light.lightPosition");
	lightColorLoc = glGetUniformLocation(basicProgram, "u_light.lightDiffuse");
	lightSpecLoc = glGetUniformLocation(basicProgram, "u_light.lightSpecular");
	ambiantStrLoc = glGetUniformLocation(basicProgram, "u_light.lightAmbient");

	attenuationConstant = glGetUniformLocation(basicProgram, "u_light.constant");
	attenuationLinear = glGetUniformLocation(basicProgram, "u_light.linear");
	attenuationQuadratic = glGetUniformLocation(basicProgram, "u_light.quadratic");

	dlightPositionLoc = glGetUniformLocation(basicProgram, "u_dLight.lightDirection");
	dlightColorLoc = glGetUniformLocation(basicProgram, "u_dLight.lightDiffuse");
	dlightSpecLoc = glGetUniformLocation(basicProgram, "u_dLight.lightSpecular");
	dambiantStrLoc = glGetUniformLocation(basicProgram, "u_dLight.lightAmbient");

	slightDirLoc = glGetUniformLocation(basicProgram, "u_sLight.lightDirection");
	slightPositionLoc = glGetUniformLocation(basicProgram, "u_sLight.lightPosition");

	slightColorLoc = glGetUniformLocation(basicProgram, "u_sLight.lightDiffuse");
	slightSpecLoc = glGetUniformLocation(basicProgram, "u_sLight.lightSpecular");
	sambiantStrLoc = glGetUniformLocation(basicProgram, "u_sLight.lightAmbient");

	scutoffLoc = glGetUniformLocation(basicProgram, "u_sLight.cutoff");
	soutCutoffLoc = glGetUniformLocation(basicProgram, "u_sLight.outerCutoff");

	skyColorLoc = glGetUniformLocation(basicProgram, "u_skyColor");
	groundColorLoc = glGetUniformLocation(basicProgram, "u_groundColor");

	viewPosLoc = glGetUniformLocation(basicProgram, "u_viewPos");

	//----Material
	matDiffLoc = glGetUniformLocation(basicProgram, "u_material.diffuse");
	matAmbLoc = glGetUniformLocation(basicProgram, "u_material.ambient");
	matSpecLoc = glGetUniformLocation(basicProgram, "u_material.specular");
	matShineLoc = glGetUniformLocation(basicProgram, "u_material.shininess");

	glUniform3fv(matDiffLoc, 1, mat->matDiffuse);
	glUniform3fv(matAmbLoc, 1, mat->matAmbiant);
	glUniform3fv(matSpecLoc, 1, mat->matSpecular);
	glUniform1f(matShineLoc, mat->matShine);

}
