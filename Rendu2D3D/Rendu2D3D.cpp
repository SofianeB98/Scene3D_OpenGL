#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#if defined(_WIN32) && defined(_MSC_VER)
#pragma comment(lib, "glfw3dll.lib")
#pragma comment(lib, "glew32s.lib")			// glew32.lib si pas GLEW_STATIC
#pragma comment(lib, "opengl32.lib")
#elif defined(__APPLE__)
#elif defined(__linux__)
#endif

#include <iostream>
#include <cmath>
#include "../common/GLShader.h"
#include "Vertex3D.h"
#include "Mat4.h"
#include "DirectionalLight.h"
#include "PointLight.h"
#include "SpotLight.h"
#include "Material.h"
#include "Model.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define M_PI 3.141592653589793238462643383
#define BUFFER_OFFSET(i) ((void*)(i))

const float WIDTH = 1280;
const float HEIGHT = 720;

#pragma region Models & Framebuffer
GLShader g_SuzShader;
uint32_t suzProgram;
GLuint VBO;
GLuint IBO;
GLuint VAO;

GLShader g_ScreenShader;
uint32_t screenProgram;
GLuint VBOscreen;
GLuint VAOscreen;

GLuint FBO;
GLuint TEX;
GLuint TEX_DEP;

//Projection Matrice
Mat4* matriceProjection = new Mat4();

//View Matrice
Mat4* matriceView = new Mat4();

//Model Matrice
Mat4* matriceRot = new Mat4();
Mat4* matriceScale = new Mat4();
Mat4* matriceTranslation = new Mat4();

Mat4* worldMatrix = new Mat4();
#pragma endregion 

#pragma region Lights
PointLight* light = new PointLight();
DirectionalLight* dLight = new DirectionalLight();
SpotLight* sLight = new SpotLight();
Material* suzanneMat = new Material();

float skyColor[] = { 0.8f, 0.8f, .8f };
float groundColor[] = { 0.3f, 0.3f, 0.3f };
#pragma endregion 

const std::string MODEL_PATH = "../data/suzanne3.obj";
const std::string MATERIAL_PATH = "../data/";

std::vector<Vertex3D> vertices;
std::vector<uint32_t> indices;

#pragma region Camera & Orbitale Parameter
float viewPos[] = { 0.f, 5.f, 5.f };
float targetPos[] = { 0.f,0.f,0.f };
float upWorld[] = { 0.f,1.f,0.f };

float distanceBetweenCamAndTarget = 0.f; //rayon

float zoomMaxValue = 15.0f;
float zoomMinValue = 1.1f;
float sensitivityZoomCam = 0.8f;

bool firstMouse = true;
float lastXPos = 0.f;
float lastYPos = 0.f;
float sensitivityMoveCam = 0.1f;
float theta = 0.f;
float phi = 0.f;
#pragma endregion 

Model* suzanneModel = new Model("../data/suzanne3.obj", "SuzanneShader.vs", "SuzanneShader.fs", "../data/");

std::vector<Model> models;

int loc_position = 0;
int normals = 0;
int texCoords = 0;
int loc_color = 0;

int matriceProjLocation = 0;
int matriceWorld = 0;
int viewLocation = 0;

int lightPositionLoc = 0;
int lightColorLoc = 0;
int lightSpecLoc = 0;
int ambiantStrLoc = 0;
int attenuationConstant = 0;
int attenuationLinear = 0;
int attenuationQuadratic = 0;

int dlightPositionLoc = 0;
int dlightColorLoc = 0;
int dlightSpecLoc = 0;
int dambiantStrLoc = 0;

int slightDirLoc = 0;
int slightPositionLoc = 0;
int slightColorLoc = 0;
int slightSpecLoc = 0;
int sambiantStrLoc = 0;
int scutoffLoc = 0;
int soutCutoffLoc = 0;

int skyColorLoc = 0;
int groundColorLoc = 0;

int viewPosLoc = 0;

int matDiffLoc = 0;
int matAmbLoc = 0;
int matSpecLoc = 0;
int matShineLoc = 0;

float aspect = WIDTH / HEIGHT;

void CheckExistingVertex(Vertex3D &v)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		if(v.equalNormal(vertices[i]) && v.equalPosition(vertices[i]) && v.equalTexcoord(vertices[i]))
		{
			indices.push_back(i);
			return;
		}
	}
	vertices.push_back(v);
	indices.push_back(vertices.size()-1);
}

void LoadModel()
{
	tinyobj::attrib_t attrib;

	std::vector<tinyobj::shape_t> shapes;
	std::vector<tinyobj::material_t> materials;
	
	std::string warn;
	std::string err;
	if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, MODEL_PATH.c_str(), MATERIAL_PATH.c_str()))
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
	
	if(materials.size() > 0)
	{
		suzanneMat->matAmbiant[0] = materials[0].ambient[0];
		suzanneMat->matAmbiant[1] = materials[0].ambient[1];
		suzanneMat->matAmbiant[2] = materials[0].ambient[2];

		suzanneMat->matDiffuse[0] = materials[0].diffuse[0];
		suzanneMat->matDiffuse[1] = materials[0].diffuse[1];
		suzanneMat->matDiffuse[2] = materials[0].diffuse[2];

		suzanneMat->matSpecular[0] = materials[0].specular[0];
		suzanneMat->matSpecular[1] = materials[0].specular[1];
		suzanneMat->matSpecular[2] = materials[0].specular[2];

		suzanneMat->matShine = materials[0].shininess;
	}
}

bool Initialize()
{
	GLenum error = glewInit();
	if (error != GLEW_OK) {
		std::cout << "erreur d'initialisation de GLEW!"
			<< std::endl;
	}

	g_SuzShader.LoadVertexShader("Basic.vs");
	g_SuzShader.LoadFragmentShader("Basic.fs");
	g_SuzShader.Create();
	suzProgram = g_SuzShader.GetProgram();
	
	g_ScreenShader.LoadVertexShader("PostRender.vs");
	g_ScreenShader.LoadFragmentShader("sepia.fs");
	g_ScreenShader.Create();
	screenProgram = g_ScreenShader.GetProgram();

	models.push_back(*suzanneModel);

	for(int i = 0; i < models.size(); i++)
	{
		glGenVertexArrays(1, &models[i].VAO);
		glGenBuffers(1, &models[i].VBO);

		glBindVertexArray(models[i].VAO);

		models[i].LoadModel();
		models[i].CreateAndLoadShader();

		glBindBuffer(GL_ARRAY_BUFFER, models[i].VBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * models[i].vertices.size(), &models[i].vertices[0], GL_STATIC_DRAW);
		glUseProgram(models[i].basicProgram);

		// ---------------- Position
		models[i].loc_position = glGetAttribLocation(models[i].basicProgram, "a_position");
		glEnableVertexAttribArray(models[i].loc_position);
		glVertexAttribPointer(models[i].loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, x)));

		// ---------------- Normals
		models[i].normals = glGetAttribLocation(models[i].basicProgram, "a_normals");
		glEnableVertexAttribArray(models[i].normals);
		glVertexAttribPointer(models[i].normals, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, nx)));

		// ---------------- Texcoord
		models[i].texCoords = glGetAttribLocation(models[i].basicProgram, "a_texcoords");
		glEnableVertexAttribArray(models[i].texCoords);
		glVertexAttribPointer(models[i].texCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, u)));

		// ---------------- Couleur
		models[i].loc_color = glGetAttribLocation(models[i].basicProgram, "a_color");
		glEnableVertexAttribArray(models[i].loc_color);
		glVertexAttribPointer(models[i].loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, r)));

		glGenBuffers(1, &models[i].IBO);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, models[i].IBO);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * models[i].indices.size(), &models[i].indices[0], GL_STATIC_DRAW);

		glBindVertexArray(0);

		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}
	
#ifdef WIN32
	wglSwapIntervalEXT(1);
#endif

	std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;

//#pragma region Model Loading
//	glGenVertexArrays(1, &suzanneModel->VAO);
//	glGenBuffers(1, &suzanneModel->VBO);
//
//	glBindVertexArray(suzanneModel->VAO);
//
//	suzanneModel->LoadModel();
//	suzanneModel->CreateAndLoadShader();
//
//	glBindBuffer(GL_ARRAY_BUFFER, suzanneModel->VBO);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * suzanneModel->vertices.size(), &suzanneModel->vertices[0], GL_STATIC_DRAW);
//	glUseProgram(suzanneModel->basicProgram);
//
//	// ---------------- Position
//	suzanneModel->loc_position = glGetAttribLocation(suzanneModel->basicProgram, "a_position");
//	glEnableVertexAttribArray(suzanneModel->loc_position);
//	glVertexAttribPointer(suzanneModel->loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, x)));
//
//	// ---------------- Normals
//	suzanneModel->normals = glGetAttribLocation(suzanneModel->basicProgram, "a_normals");
//	glEnableVertexAttribArray(suzanneModel->normals);
//	glVertexAttribPointer(suzanneModel->normals, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, nx)));
//
//	// ---------------- Texcoord
//	suzanneModel->texCoords = glGetAttribLocation(suzanneModel->basicProgram, "a_texcoords");
//	glEnableVertexAttribArray(suzanneModel->texCoords);
//	glVertexAttribPointer(suzanneModel->texCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, u)));
//
//	// ---------------- Couleur
//	suzanneModel->loc_color = glGetAttribLocation(suzanneModel->basicProgram, "a_color");
//	glEnableVertexAttribArray(suzanneModel->loc_color);
//	glVertexAttribPointer(suzanneModel->loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, r)));
//
//	glGenBuffers(1, &suzanneModel->IBO);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, suzanneModel->IBO);
//	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * suzanneModel->indices.size(), &suzanneModel->indices[0], GL_STATIC_DRAW);
//
//	glBindVertexArray(0);
//
//	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
//	glBindBuffer(GL_ARRAY_BUFFER, 0);
//#pragma endregion 
	
#pragma region Initialisation_Du_Singe
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	glUseProgram(suzProgram);

	LoadModel();

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// ---------------- Position
	loc_position = glGetAttribLocation(suzProgram, "a_position");
	glEnableVertexAttribArray(loc_position);
	glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, x)));

	// ---------------- Normals
	normals = glGetAttribLocation(suzProgram, "a_normals");
	glEnableVertexAttribArray(normals);
	glVertexAttribPointer(normals, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, nx)));
	
	// ---------------- Texcoord
	texCoords = glGetAttribLocation(suzProgram, "a_texcoords");
	glEnableVertexAttribArray(texCoords);
	glVertexAttribPointer(texCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, u)));
	
	// ---------------- Couleur
	loc_color = glGetAttribLocation(suzProgram, "a_color");
	glEnableVertexAttribArray(loc_color);
	glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, r)));

	//--IBO
	glGenBuffers(1, &IBO);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
	

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion

#pragma region ScreenPP
	//Quad qui va prendre tout l'ecran !
	float quadVertices[] = { 
		// positions   // texCoords
		-1.0f,  1.0f,  0.0f, 1.0f,
		-1.0f, -1.0f,  0.0f, 0.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,

		-1.0f,  1.0f,  0.0f, 1.0f,
		 1.0f, -1.0f,  1.0f, 0.0f,
		 1.0f,  1.0f,  1.0f, 1.0f
	};
	glGenVertexArrays(1, &VAOscreen);
	glGenBuffers(1, &VBOscreen);
	glBindVertexArray(VAOscreen);
	glBindBuffer(GL_ARRAY_BUFFER, VBOscreen);
	glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
	
	loc_position = glGetAttribLocation(screenProgram, "a_position");
	glEnableVertexAttribArray(loc_position);
	glVertexAttribPointer(loc_position, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), BUFFER_OFFSET(0));

	texCoords = glGetAttribLocation(screenProgram, "a_texcoords");
	glEnableVertexAttribArray(texCoords);
	glVertexAttribPointer(texCoords, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), BUFFER_OFFSET(2 * sizeof(float)));

	glBindVertexArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
#pragma endregion 
	
#pragma region FBO
	glGenFramebuffers(1, &FBO);
	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glGenTextures(1, &TEX);
	glBindTexture(GL_TEXTURE_2D, TEX);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, WIDTH, HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, TEX, 0);

	glGenTextures(1, &TEX_DEP);
	glBindTexture(GL_TEXTURE_2D, TEX_DEP);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, WIDTH, HEIGHT, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, NULL);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, TEX_DEP, 0);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR FRAME BUFFER NOT ALLOCATED !!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
#pragma endregion
	
	return true;
}

void Shutdown()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteVertexArrays(1, &VAO);
	
	glDeleteFramebuffers(1, &FBO);
	glDeleteTextures(1, &TEX_DEP);
	glDeleteTextures(1, &TEX);

	glDeleteBuffers(1, &VBOscreen);
	glDeleteVertexArrays(1, &VAOscreen);
	
	g_SuzShader.Destroy();
	g_ScreenShader.Destroy();

	for(int i = 0; i < models.size(); i ++)
	{
		glDeleteBuffers(1, &models[i].VBO);
		glDeleteBuffers(1, &models[i].IBO);
		glDeleteVertexArrays(1, &models[i].VAO);
		models[i].g_BasicShader.Destroy();
	}

	models.empty();
}

void Render(GLFWwindow* window)
{
	matriceView->LookAt(viewPos, targetPos, upWorld);
	
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthMask(GL_TRUE);
	glFrontFace(GL_CCW);
	glCullFace(GL_BACK);
	glDepthFunc(GL_LESS);

	glBindFramebuffer(GL_FRAMEBUFFER, FBO);

	glViewport(0, 0, WIDTH, HEIGHT);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(suzProgram);
	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

	// --- Suzanne
	glUniformMatrix4fv(matriceWorld, 1, false, worldMatrix->matrice);

	glUniformMatrix4fv(viewLocation, 1, false, matriceView->matrice);
	glUniform3fv(viewPosLoc, 1, viewPos);

	glUniform3fv(lightPositionLoc, 1, light->lightPosition);

	glUniform3fv(dlightPositionLoc, 1, dLight->lightDirection);

	glUniform3fv(slightPositionLoc, 1, sLight->lightPosition);
	glUniform3fv(slightDirLoc, 1, sLight->lightDirection);
	// --- Suzanne
	
	for(int i = 0; i < models.size(); i++)
	{
		glUseProgram(models[i].basicProgram);

		glBindVertexArray(models[i].VAO);
		glDrawElements(GL_TRIANGLES, models[i].indices.size(), GL_UNSIGNED_INT, 0);

		// --- Model
		glUniformMatrix4fv(models[i].matriceProjLocation, 1, false, matriceProjection->matrice);
		glUniformMatrix4fv(models[i].matriceWorld, 1, false, models[i].worldMatrix->matrice);

		glUniformMatrix4fv(models[i].viewLocation, 1, false, matriceView->matrice);
		glUniform3fv(models[i].viewPosLoc, 1, viewPos);

		glUniform3fv(models[i].lightPositionLoc, 1, light->lightPosition);

		glUniform3fv(models[i].dlightPositionLoc, 1, dLight->lightDirection);

		glUniform3fv(models[i].slightPositionLoc, 1, sLight->lightPosition);
		glUniform3fv(models[i].slightDirLoc, 1, sLight->lightDirection);

		glUniform3fv(models[i].lightColorLoc, 1, light->lightDiffuse);
		glUniform3fv(models[i].lightSpecLoc, 1, light->lightSpecular);

		glUniform1f(models[i].attenuationConstant, light->constant);
		glUniform1f(models[i].attenuationLinear, light->linear);
		glUniform1f(models[i].attenuationQuadratic, light->quadratic);

		glUniform3fv(models[i].ambiantStrLoc, 1, light->lightAmbient);
		glUniform3fv(models[i].dlightColorLoc, 1, dLight->lightDiffuse);
		glUniform3fv(models[i].dlightSpecLoc, 1, dLight->lightSpecular);
		glUniform3fv(models[i].dambiantStrLoc, 1, dLight->lightAmbient);

		glUniform3fv(models[i].slightColorLoc, 1, sLight->lightDiffuse);
		glUniform3fv(models[i].slightSpecLoc, 1, sLight->lightSpecular);
		glUniform3fv(models[i].sambiantStrLoc, 1, sLight->lightAmbient);

		glUniform1f(models[i].scutoffLoc, sLight->cutoff);
		glUniform1f(models[i].soutCutoffLoc, sLight->outerCutoff);

		glUniform3fv(models[i].skyColorLoc, 1, skyColor);
		glUniform3fv(models[i].groundColorLoc, 1, groundColor);

		glUniform3fv(models[i].matDiffLoc, 1, models[i].mat->matDiffuse);
		glUniform3fv(models[i].matAmbLoc, 1, models[i].mat->matAmbiant);
		glUniform3fv(models[i].matSpecLoc, 1, models[i].mat->matSpecular);
		glUniform1f(models[i].matShineLoc, models[i].mat->matShine);
	}
	
	// --- Model
	
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
}

void RenderPostPro(GLFWwindow * window)
{
	glDisable(GL_DEPTH_TEST);
	glDepthMask(GL_FALSE);
	glEnable(GL_FRAMEBUFFER_SRGB);
	glClear(GL_COLOR_BUFFER_BIT);
	
	glUseProgram(screenProgram);
	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAOscreen);
	glBindTexture(GL_TEXTURE_2D, TEX);
	glDrawArrays(GL_TRIANGLES, 0, 6);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	if (distanceBetweenCamAndTarget >= zoomMaxValue - 0.1f && yoffset < 0)
		return;

	if (distanceBetweenCamAndTarget <= zoomMinValue + 0.1f && yoffset > 0)
		return;

	distanceBetweenCamAndTarget -= yoffset * sensitivityZoomCam;
	
	float newTheta = theta * M_PI / 180.0f;
	float newPhi = phi * M_PI / 180.0f;

	viewPos[0] = distanceBetweenCamAndTarget * std::cos(newTheta) * std::cos(newPhi);
	viewPos[1] = distanceBetweenCamAndTarget * std::sin(newTheta);
	viewPos[2] = distanceBetweenCamAndTarget * std::cos(newTheta) * std::sin(newPhi);
}

void cursor_callback(GLFWwindow* window, double xpos, double ypos)
{
	if(firstMouse)
	{
		lastXPos = xpos;
		lastYPos = ypos;
		firstMouse = false;
	}

	float xoffset = xpos - lastXPos;
	float yoffset = lastYPos - ypos;
	lastXPos = xpos;
	lastYPos = ypos;

	xoffset *= sensitivityMoveCam;
	yoffset *= sensitivityMoveCam;

	theta += yoffset;
	phi += xoffset;

	if (phi > 180.0f)
		phi = -180.0f;
	if (phi < -180.0f)
		phi = 180.0f;
	
	if (theta > 89.0f)
		theta = 89.0;
	if (theta < -89.0f)
		theta = -89.0;
	
	float newTheta = theta * M_PI / 180.0f;
	float newPhi = phi * M_PI / 180.0f;
	
	viewPos[0] = distanceBetweenCamAndTarget *std::cos(newTheta) * std::cos(newPhi);
	viewPos[1] = distanceBetweenCamAndTarget *std::sin(newTheta);
	viewPos[2] = distanceBetweenCamAndTarget *std::cos(newTheta) * std::sin(newPhi);

}

int main(void)
{
	GLFWwindow* window;

	/* Initialize the library */
	if (!glfwInit())
		return -1;

	/* Create a windowed mode window and its OpenGL context */
	
	window = glfwCreateWindow(WIDTH, HEIGHT, "AFFICHAGE .OBJ", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwSetCursorPosCallback(window, cursor_callback); //permet de gérer les movement de la souris et d'executer du script
	glfwSetScrollCallback(window, scroll_callback); //permet de gérer le scroll de la souris et d'executer du script

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // permet d'utiliser les input de la souris
	/* Make the window's context current */
	glfwMakeContextCurrent(window);

	// toutes nos initialisations vont ici
	if (!Initialize())
	{
		return -1;
	}

	suzProgram = g_SuzShader.GetProgram();
	
#pragma region MATRICES

	matriceProjLocation = glGetUniformLocation(suzProgram, "u_matriceProjection");
	matriceWorld = glGetUniformLocation(suzProgram, "u_worldMatrix");
	viewLocation = glGetUniformLocation(suzProgram, "u_view");
	
	aspect = WIDTH / HEIGHT;

	matriceProjection->Perspective(45, aspect, 0.1f, 1000.0f);
	glUniformMatrix4fv(matriceProjLocation, 1, false, matriceProjection->matrice);
	
	matriceView->LookAt(viewPos, targetPos, upWorld);
	glUniformMatrix4fv(viewLocation, 1, false, matriceView->matrice);

	matriceScale->Scale(1.f);
	matriceTranslation->Translate(0, 0, .0f);
	matriceRot->RotateY(0);
	
	worldMatrix = worldMatrix->Multiply(*matriceScale, *matriceRot);
	worldMatrix = worldMatrix->Multiply(*worldMatrix, *matriceTranslation);
	glUniformMatrix4fv(matriceWorld, 1, false, worldMatrix->matrice);
	
#pragma endregion

#pragma region LIGHT
	// --- Point light
	lightPositionLoc = glGetUniformLocation(suzProgram, "u_light.lightPosition");
	lightColorLoc = glGetUniformLocation(suzProgram, "u_light.lightDiffuse");
	lightSpecLoc = glGetUniformLocation(suzProgram, "u_light.lightSpecular");
	ambiantStrLoc = glGetUniformLocation(suzProgram, "u_light.lightAmbient");
	
	attenuationConstant = glGetUniformLocation(suzProgram, "u_light.constant");
	attenuationLinear = glGetUniformLocation(suzProgram, "u_light.linear");
	attenuationQuadratic = glGetUniformLocation(suzProgram, "u_light.quadratic");
	
	light->setPosition(-2.0f, 2.0f, -2.0f);
	light->setAmbiant(0.86f, 0.765f, 0.1f);
	
	glUniform3fv(lightPositionLoc, 1, light->lightPosition);
	glUniform3fv(lightColorLoc, 1, light->lightDiffuse);
	glUniform3fv(lightSpecLoc, 1, light->lightSpecular);

	glUniform1f(attenuationConstant, light->constant);
	glUniform1f(attenuationLinear, light->linear);
	glUniform1f(attenuationQuadratic, light->quadratic);
	
	glUniform3fv(ambiantStrLoc, 1, light->lightAmbient);
	// --- End point light

	// --- Directional light
	dlightPositionLoc = glGetUniformLocation(suzProgram, "u_dLight.lightDirection");
	dlightColorLoc = glGetUniformLocation(suzProgram, "u_dLight.lightDiffuse");
	dlightSpecLoc = glGetUniformLocation(suzProgram, "u_dLight.lightSpecular");
	dambiantStrLoc = glGetUniformLocation(suzProgram, "u_dLight.lightAmbient");

	dLight->setDirection(1.f, 1.0f, -1.f);
	
	glUniform3fv(dlightPositionLoc, 1, dLight->lightDirection);
	glUniform3fv(dlightColorLoc, 1, dLight->lightDiffuse);
	glUniform3fv(dlightSpecLoc, 1, dLight->lightSpecular);
	glUniform3fv(dambiantStrLoc, 1, dLight->lightAmbient);
	// --- End directional light

	// --- Spot Light
	slightDirLoc = glGetUniformLocation(suzProgram, "u_sLight.lightDirection");
	slightPositionLoc = glGetUniformLocation(suzProgram, "u_sLight.lightPosition");
	
	slightColorLoc = glGetUniformLocation(suzProgram, "u_sLight.lightDiffuse");
	slightSpecLoc = glGetUniformLocation(suzProgram, "u_sLight.lightSpecular");
	sambiantStrLoc = glGetUniformLocation(suzProgram, "u_sLight.lightAmbient");

	scutoffLoc = glGetUniformLocation(suzProgram, "u_sLight.cutoff");
	soutCutoffLoc = glGetUniformLocation(suzProgram, "u_sLight.outerCutoff");
	
	sLight->setPosition(0,0,5.0f);
	sLight->setDirection(0,0,-1.0f);
	sLight->cutoff = std::cos(5.0f * 0.0174533f);
	sLight->outerCutoff = std::cos((8.50f) * 0.0174533f);
	
	glUniform3fv(slightPositionLoc, 1, sLight->lightPosition);
	glUniform3fv(slightDirLoc, 1, sLight->lightDirection);
	
	glUniform3fv(slightColorLoc, 1, sLight->lightDiffuse);
	glUniform3fv(slightSpecLoc, 1, sLight->lightSpecular);
	glUniform3fv(sambiantStrLoc, 1, sLight->lightAmbient);
	
	glUniform1f(scutoffLoc, sLight->cutoff);
	glUniform1f(soutCutoffLoc, sLight->outerCutoff);
	// --- End spot light

	// --- Sky & Ground
	skyColorLoc= glGetUniformLocation(suzProgram, "u_skyColor");
	groundColorLoc = glGetUniformLocation(suzProgram, "u_groundColor");

	glUniform3fv(skyColorLoc, 1, skyColor);
	glUniform3fv(groundColorLoc, 1, groundColor);
	// --- End Sky & ground
#pragma endregion

	viewPosLoc = glGetUniformLocation(suzProgram, "u_viewPos");
	glUniform3fv(viewPosLoc, 1, viewPos);
	
#pragma region MATERIAL
	matDiffLoc = glGetUniformLocation(suzProgram, "u_material.diffuse");
	matAmbLoc = glGetUniformLocation(suzProgram, "u_material.ambient");
	matSpecLoc = glGetUniformLocation(suzProgram, "u_material.specular");
	matShineLoc = glGetUniformLocation(suzProgram, "u_material.shininess");
	
	glUniform3fv(matDiffLoc, 1, suzanneMat->matDiffuse);
	glUniform3fv(matAmbLoc, 1, suzanneMat->matAmbiant);
	glUniform3fv(matSpecLoc, 1, suzanneMat->matSpecular);
	glUniform1f(matShineLoc, suzanneMat->matShine);
#pragma endregion 

	distanceBetweenCamAndTarget = std::sqrt(std::pow(viewPos[0] - targetPos[0], 2) +
		std::pow(viewPos[1] - targetPos[1], 2) +
		std::pow(viewPos[2] - targetPos[2], 2));

	//Boucle d'initialization de toutes les loc des model 

	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		/* Render here */
		Render(window);

		RenderPostPro(window);
		
		/* Swap front and back buffers */
		glfwSwapBuffers(window);

		/* Poll for and process events */
		glfwPollEvents();
	}
	
	// ne pas oublier de liberer la memoire etc...
	Shutdown();

	glfwTerminate();
	return 0;
}