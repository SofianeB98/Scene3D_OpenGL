//
//
//

// GLEW_STATIC force le linkage statique
// c-a-d que le code de glew est directement injecte dans l'executable
#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

// les repertoires d'includes sont:
// ../libs/glfw-3.3/include			fenetrage
// ../libs/glew-2.1.0/include		extensions OpenGL
// ../libs/stb						gestion des images (entre autre)

// les repertoires des libs sont (en 64-bit):
// ../libs/glfw-3.3/lib-vc2015
// ../libs/glew-2.1.0/lib/Release/x64

// Pensez a copier les dll dans le repertoire x64/Debug, cad:
// glfw-3.3/lib-vc2015/glfw3.dll
// glew-2.1.0/bin/Release/x64/glew32.dll		si pas GLEW_STATIC

// _WIN32 indique un programme Windows
// _MSC_VER indique la version du compilateur VC++
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

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#define M_PI 3.141592653589793238462643383
#define BUFFER_OFFSET(i) ((void*)(i))

const float WIDTH = 1280;
const float HEIGHT = 720;

GLShader g_BasicShader;
GLuint VBO;
GLuint IBO;
GLuint VAO;

//Projection Matrice
Mat4* matriceProjection = new Mat4();

//View Matrice
Mat4* matriceView = new Mat4();

//Model Matrice
Mat4* matriceRot = new Mat4();
Mat4* matriceScale = new Mat4();
Mat4* matriceTranslation = new Mat4();

Mat4* worldMatrix = new Mat4();

PointLight* light = new PointLight();
DirectionalLight* dLight = new DirectionalLight();
SpotLight* sLight = new SpotLight();
Material* suzanneMat = new Material();

float skyColor[] = { 0.8f, 0.8f, .8f };
float groundColor[] = { 0.3f, 0.3f, 0.3f };

const std::string MODEL_PATH = "../data/suzanne3.obj";
const std::string MATERIAL_PATH = "../data/";

std::vector<Vertex3D> vertices;
std::vector<uint32_t> indices;

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

		std::cout << "# shniness " << materials[0].shininess;
		
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

	g_BasicShader.LoadVertexShader("Basic.vs");
	g_BasicShader.LoadFragmentShader("Basic.fs");
	g_BasicShader.Create();
	
#ifdef WIN32
	wglSwapIntervalEXT(1);
#endif

	std::cout << "Version : " << glGetString(GL_VERSION) << std::endl;
	std::cout << "Vendor : " << glGetString(GL_VENDOR) << std::endl;
	std::cout << "Renderer : " << glGetString(GL_RENDERER) << std::endl;

	glClearColor(0.5f, 0.5f, 0.5f, 1.f);
	glEnable(GL_DEPTH_TEST);
	
	glFrontFace(GL_CCW);
	glCullFace(GL_FRONT);
	
	glDepthFunc(GL_LESS);
	
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);
	
	#pragma region Initialisation_Du_Triangle
	
	uint32_t basicProgram = g_BasicShader.GetProgram();
	glUseProgram(basicProgram);

	LoadModel();

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex3D) * vertices.size(), &vertices[0], GL_STATIC_DRAW);

	// ---------------- Position
	int loc_position = glGetAttribLocation(basicProgram, "a_position");
	glEnableVertexAttribArray(loc_position);
	glVertexAttribPointer(loc_position, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, x)));

	// ---------------- Normals
	int normals = glGetAttribLocation(basicProgram, "a_normals");
	glEnableVertexAttribArray(normals);
	glVertexAttribPointer(normals, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, nx)));
	
	// ---------------- Texcoord
	int texCoords = glGetAttribLocation(basicProgram, "a_texcoords");
	glEnableVertexAttribArray(texCoords);
	glVertexAttribPointer(texCoords, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, u)));
	
	// ---------------- Couleur
	int loc_color = glGetAttribLocation(basicProgram, "a_color");
	glEnableVertexAttribArray(loc_color);
	glVertexAttribPointer(loc_color, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex3D), BUFFER_OFFSET(offsetof(Vertex3D, r)));

	#pragma endregion
	
	#pragma region Initialisation_Du_IndexBuffer
	glGenBuffers(1, &IBO);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * indices.size(), &indices[0], GL_STATIC_DRAW);
	#pragma endregion

	glBindVertexArray(0);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
		
	return true;
}

void Shutdown()
{
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &IBO);
	glDeleteVertexArrays(1, &VAO);
	g_BasicShader.Destroy();
}

void Render(GLFWwindow* window)
{
	// etape a. A vous de recuperer/passer les variables WIDTH/HEIGHT
	glViewport(0, 0, WIDTH, HEIGHT);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	//-----------------
	//glBindBuffer(GL_ARRAY_BUFFER, VBO);
	
	//-----------------
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO);

	glBindVertexArray(VAO);
	
	// dessine un triangle
	glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	//float forward[3];

	//float distance = 0.f;

	//distanceBetweenCamAndTarget = std::sqrt(std::pow(viewPos[0] - targetPos[0], 2) +
	//	std::pow(viewPos[1] - targetPos[1], 2) + 
	//	std::pow(viewPos[2] - targetPos[2], 2));
	//
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
	
	//
	//for(int i = 0; i < 3; i++)
	//{
	//	forward[i] = matriceView->matrice[i * 4 + 2 ];
	//	viewPos[i] -= forward[i] * yoffset * sensitivityZoomCam;
	//}
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

	uint32_t basicProgram = g_BasicShader.GetProgram();
	glUseProgram(basicProgram);
	
	
	
#pragma region MATRICES

	int matriceProjLocation = glGetUniformLocation(basicProgram, "u_matriceProjection");
	int matriceWorld = glGetUniformLocation(basicProgram, "u_worldMatrix");
	int viewLocation = glGetUniformLocation(basicProgram, "u_view");
	
	float aspect = WIDTH / HEIGHT;

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
	int lightPositionLoc = glGetUniformLocation(basicProgram, "u_light.lightPosition");
	int lightColorLoc = glGetUniformLocation(basicProgram, "u_light.lightDiffuse");
	int lightSpecLoc = glGetUniformLocation(basicProgram, "u_light.lightSpecular");
	int ambiantStrLoc = glGetUniformLocation(basicProgram, "u_light.lightAmbient");
	
	int attenuationConstant = glGetUniformLocation(basicProgram, "u_light.constant");
	int attenuationLinear = glGetUniformLocation(basicProgram, "u_light.linear");
	int attenuationQuadratic = glGetUniformLocation(basicProgram, "u_light.quadratic");
	
	light->setPosition(-2.0f, 2.0f, -2.0f);
	
	glUniform3fv(lightPositionLoc, 1, light->lightPosition);
	glUniform3fv(lightColorLoc, 1, light->lightDiffuse);
	glUniform3fv(lightSpecLoc, 1, light->lightSpecular);

	glUniform1f(attenuationConstant, light->constant);
	glUniform1f(attenuationLinear, light->linear);
	glUniform1f(attenuationQuadratic, light->quadratic);
	
	glUniform3fv(ambiantStrLoc, 1, light->lightAmbient);
	// --- End point light

	// --- Directional light
	int dlightPositionLoc = glGetUniformLocation(basicProgram, "u_dLight.lightDirection");
	int dlightColorLoc = glGetUniformLocation(basicProgram, "u_dLight.lightDiffuse");
	int dlightSpecLoc = glGetUniformLocation(basicProgram, "u_dLight.lightSpecular");
	int dambiantStrLoc = glGetUniformLocation(basicProgram, "u_dLight.lightAmbient");

	dLight->setDirection(1.f, 1.0f, -1.f);
	
	glUniform3fv(dlightPositionLoc, 1, dLight->lightDirection);
	glUniform3fv(dlightColorLoc, 1, dLight->lightDiffuse);
	glUniform3fv(dlightSpecLoc, 1, dLight->lightSpecular);
	glUniform3fv(dambiantStrLoc, 1, dLight->lightAmbient);
	// --- End directional light

	// --- Spot Light
	int slightDirLoc = glGetUniformLocation(basicProgram, "u_sLight.lightDirection");
	int slightPositionLoc = glGetUniformLocation(basicProgram, "u_sLight.lightPosition");
	
	int slightColorLoc = glGetUniformLocation(basicProgram, "u_sLight.lightDiffuse");
	int slightSpecLoc = glGetUniformLocation(basicProgram, "u_sLight.lightSpecular");
	int sambiantStrLoc = glGetUniformLocation(basicProgram, "u_sLight.lightAmbient");

	int scutoffLoc = glGetUniformLocation(basicProgram, "u_sLight.cutoff");
	int soutCutoffLoc = glGetUniformLocation(basicProgram, "u_sLight.outerCutoff");
	
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
	int skyColorLoc= glGetUniformLocation(basicProgram, "u_skyColor");
	int groundColorLoc = glGetUniformLocation(basicProgram, "u_groundColor");

	glUniform3fv(skyColorLoc, 1, skyColor);
	glUniform3fv(groundColorLoc, 1, groundColor);
	// --- End Sky & ground
#pragma endregion

	int viewPosLoc = glGetUniformLocation(basicProgram, "u_viewPos");
	glUniform3fv(viewPosLoc, 1, viewPos);
	
#pragma region MATERIAL
	int matDiffLoc = glGetUniformLocation(basicProgram, "u_material.diffuse");
	int matAmbLoc = glGetUniformLocation(basicProgram, "u_material.ambient");
	int matSpecLoc = glGetUniformLocation(basicProgram, "u_material.specular");
	int matShineLoc = glGetUniformLocation(basicProgram, "u_material.shininess");
	
	glUniform3fv(matDiffLoc, 1, suzanneMat->matDiffuse);
	glUniform3fv(matAmbLoc, 1, suzanneMat->matAmbiant);
	glUniform3fv(matSpecLoc, 1, suzanneMat->matSpecular);
	glUniform1f(matShineLoc, suzanneMat->matShine);
#pragma endregion 

	distanceBetweenCamAndTarget = std::sqrt(std::pow(viewPos[0] - targetPos[0], 2) +
		std::pow(viewPos[1] - targetPos[1], 2) +
		std::pow(viewPos[2] - targetPos[2], 2));
	
	/* Loop until the user closes the window */
	while (!glfwWindowShouldClose(window))
	{
		matriceView->LookAt(viewPos, targetPos, upWorld);
		glUniformMatrix4fv(viewLocation, 1, false, matriceView->matrice);

		//worldMatrix->RotateY(-30 * 3.14159265f / 180.0f * glfwGetTime());
		//glUniformMatrix4fv(matriceWorld, 1, false, worldMatrix->matrice);

		/* Render here */
		Render(window);

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