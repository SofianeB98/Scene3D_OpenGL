#pragma once

struct Vertex3D
{
	float x, y, z;
	float nx, ny, nz;
	float u, v;
	float r, g, b;

	bool equalPosition(Vertex3D& v);
	bool equalNormal(Vertex3D& v);
	bool equalTexcoord(Vertex3D& v);
};
