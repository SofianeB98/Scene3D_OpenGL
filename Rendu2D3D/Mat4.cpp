#include "Mat4.h"
#include <cmath>

#define M_PI 3.141592653589793238462643383

Mat4::Mat4()
{
	for (int i = 0;i < this->matriceSizeHeight; i++)
	{
		for(int j = 0; j < this->matriceSizeWidth; j++)
		{
			this->matrice[i * this->matriceSizeHeight + j] = (i == j ? 1 : 0);
		}
	}
}

Mat4::~Mat4()
{
	delete[] this->matrice;
}

void Mat4::RotateZ(float angle)
{
	this->matrice[0 * this->matriceSizeHeight + 0] = cos(angle);
	this->matrice[0 * this->matriceSizeHeight + 1] = sin(angle);
	this->matrice[1 * this->matriceSizeHeight + 0] = -sin(angle);
	this->matrice[1 * this->matriceSizeHeight + 1] = cos(angle);
}

void Mat4::RotateX(float angle)
{
	this->matrice[1 * this->matriceSizeHeight + 1] = cos(angle);
	this->matrice[1 * this->matriceSizeHeight + 2] = sin(angle);
	this->matrice[2 * this->matriceSizeHeight + 1] = -sin(angle);
	this->matrice[2 * this->matriceSizeHeight + 2] = cos(angle);
}

void Mat4::RotateY(float angle)
{
	this->matrice[0 * this->matriceSizeHeight + 0] = cos(angle);
	this->matrice[2 * this->matriceSizeHeight + 0] = sin(angle);
	this->matrice[0 * this->matriceSizeHeight + 2] = -sin(angle);
	this->matrice[2 * this->matriceSizeHeight + 2] = cos(angle);
}

void Mat4::Scale(float factor)
{
	for (int i = 0; i < this->matriceSizeHeight; i++)
	{
		for (int j = 0; j < this->matriceSizeWidth; j++)
		{
			if (i == this->matriceSizeHeight - 1)
				break;
			
			this->matrice[i * this->matriceSizeHeight + j] = (i == j ? factor : this->matrice[i * this->matriceSizeHeight + j]);
		}
	}
}

void Mat4::SetProjection(float windowWidth, float windowHeight, float near, float far)
{
	this->matrice[0 * this->matriceSizeHeight + 0] = 1 / windowWidth;
	this->matrice[1 * this->matriceSizeHeight + 1] = 1 / windowHeight;
}

void Mat4::Translate(float x, float y, float z)
{
	this->matrice[3 * this->matriceSizeHeight + 0] = x;
	this->matrice[3 * this->matriceSizeHeight + 1] = y;
	this->matrice[3 * this->matriceSizeHeight + 2] = z;
}

void Mat4::Ortho(float left, float right, float bottom, float top, float znear, float zfar)
{
	this->matrice[0 * this->matriceSizeHeight + 0] = 2 / (right - left);
	this->matrice[1 * this->matriceSizeHeight + 1] = 2 / (top - bottom);
	this->matrice[2 * this->matriceSizeHeight + 2] = -2 / (zfar - znear);

	this->matrice[3 * this->matriceSizeHeight + 0] = -((right + left) / (right - left));
	this->matrice[3 * this->matriceSizeHeight + 1] = -((top + bottom) / (top - bottom));
	this->matrice[3 * this->matriceSizeHeight + 2] = -((zfar + znear) / (zfar - znear));
}

void Mat4::Perspective(float fov, float aspect, float near, float far)
{
	float radFov = fov * M_PI / 180.0f;
	float f = 1 / tan(radFov / 2);
	
	this->matrice[0 * this->matriceSizeHeight + 0] = f/aspect;
	this->matrice[1 * this->matriceSizeHeight + 1] = f;
	this->matrice[2 * this->matriceSizeHeight + 2] = -far/(far-near);
	this->matrice[2 * this->matriceSizeHeight + 3] = -1;

	this->matrice[3 * this->matriceSizeHeight + 2] = -near * far / (far - near);
}

void Mat4::LookAt(float* cPosition, float* targetPosition, float* upWorld)
{
	float forward[3];
	for(int i = 0; i < 3; i++)
	{
		forward[i] = targetPosition[i] - cPosition[i];
	}
	
	float forwardLength = std::sqrt(std::pow(forward[0], 2) + std::pow(forward[1], 2) + std::pow(forward[2], 2));
	
	for (int i = 0; i < 3; i++)
	{
		forward[i] *= -1.f;
		forward[i] /= forwardLength;
	}

	float right[3];

	right[0] = upWorld[1] * forward[2] - upWorld[2] * forward[1];
	right[1] = upWorld[2] * forward[0] - upWorld[0] * forward[2];
	right[2] = upWorld[0] * forward[1] - upWorld[1] * forward[0];

	float rLength = std::sqrt(std::pow(right[0], 2) + std::pow(right[1], 2) + std::pow(right[2], 2));

	for (int i = 0; i < 3; i++)
	{
		right[i] /= rLength;
	}


	float up[3];

	up[0] = forward[1] * right[2] - forward[2] * right[1];
	up[1] = forward[2] * right[0] - forward[0] * right[2];
	up[2] = forward[0] * right[1] - forward[1] * right[0];

	float uLength = std::sqrt(std::pow(up[0], 2) + std::pow(up[1], 2) + std::pow(up[2], 2));

	for (int i = 0; i < 3; i++)
	{
		up[i] /= uLength;
	}
	
	float x = right[0] * cPosition[0] + right[1] * cPosition[1] + right[2] * cPosition[2];
	
	float y = up[0] * cPosition[0] + up[1] * cPosition[1] + up[2] * cPosition[2];
	
	float z = forward[0] * cPosition[0] + forward[1] * cPosition[1] + forward[2] * cPosition[2];

	this->matrice[0] = right[0];
	this->matrice[1] = up[0];
	this->matrice[2] = forward[0];
	this->matrice[3] = 0.f;

	this->matrice[4] = right[1];
	this->matrice[5] = up[1];
	this->matrice[6] = forward[1];
	this->matrice[7] = 0.f;

	this->matrice[8] = right[2];
	this->matrice[9] = up[2];
	this->matrice[10] = forward[2];
	this->matrice[11] = 0.f;

	this->matrice[12] = -x;
	this->matrice[13] = -y;
	this->matrice[14] = -z;
	this->matrice[15] = 1;
}

Mat4 *Mat4::Multiply(const Mat4& m1, const Mat4& m2)
{
	Mat4* mat = new Mat4();
	int k = 0;

	for (int i = 0; i < this->matriceSizeHeight; i++)
	{
		for (int j = 0; j < this->matriceSizeWidth; j++)
		{
			mat->matrice[i * mat->matriceSizeHeight+ j] = 
										m1.matrice[i * m1.matriceSizeWidth + 0] * m2.matrice[0 * m2.matriceSizeWidth + j] +
										m1.matrice[i * m1.matriceSizeWidth + 1] * m2.matrice[1 * m2.matriceSizeWidth + j] + 
										m1.matrice[i * m1.matriceSizeWidth + 2] * m2.matrice[2 * m2.matriceSizeWidth + j] + 
										m1.matrice[i * m1.matriceSizeWidth + 3] * m2.matrice[3 * m2.matriceSizeWidth + j];
		}
	}

	return mat;
}
