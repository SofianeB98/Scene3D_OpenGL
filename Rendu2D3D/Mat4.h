#pragma once

struct Mat4
{
	int matriceSizeHeight = 4;
	int matriceSizeWidth = 4;
	
	float matrice[16];

	Mat4(); //constructeur
	~Mat4(); //destructeur

	void RotateZ(float angle);
	void RotateX(float angle);
	void RotateY(float angle);
	
	void Scale(float factor);

	void Translate(float x, float y, float z);
	
	void SetProjection(float windowWidth, float windowHeight, float near, float far);
	void Ortho(float left, float right, float bottom, float top, float znear, float zfar);
	void Perspective(float fov, float aspect, float near, float far);

	void LookAt(float* cPosition, float* targetPosition, float* upWorld);
	
	Mat4* Multiply(const Mat4& m1, const Mat4& m2);
};