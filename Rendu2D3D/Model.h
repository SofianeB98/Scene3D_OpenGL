﻿#pragma once

#include <GL/glew.h>
#include <cstdint>
#include "../common/GLShader.h"
#include "Mat4.h"
#include <vector>
#include "Vertex3D.h"
#include "Material.h"

class Model
{
	public:
		//Model loading
		char* modelPath;
		char* materialPath;
		std::vector<Vertex3D> vertices;
		std::vector<uint32_t> indices;
	
		//Model material
		Material* mat;
		//Texture tex;
	
		//Model Shader
		GLShader g_BasicShader;
		char* vShaderName;
		char* fShaderName;
		uint32_t basicProgram;

		//Model object (VBO, IBO, VAO)
		GLuint VBO;
		GLuint IBO;
		GLuint VAO;

		//Model local Matrice
		Mat4* matriceRot;
		Mat4* matriceScale;
		Mat4* matriceTranslation;

		//Model World Matice
		Mat4* worldMatrix;

		Model();
		Model(char* modelPath, char* vShaderName, char* fShaderName, char* materialPath);
		
		~Model()
		{
			delete modelPath;
			delete materialPath;
			
			delete matriceRot;
			delete matriceScale;
			delete matriceTranslation;
			
			delete worldMatrix;
			
			delete mat;
		}
	
		void LoadModel();
		void CheckExistingVertex(Vertex3D& v);
		void CreateAndLoadShader();
};
