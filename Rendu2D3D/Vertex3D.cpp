#include "Vertex3D.h"


bool Vertex3D::equalPosition(Vertex3D& v)
{
	return (this->x == v.x && this->y == v.y && this->z == v.z);
}

bool Vertex3D::equalNormal(Vertex3D& v)
{
	return (this->nx == v.nx && this->ny == v.ny && this->nz == v.nz);
}

bool Vertex3D::equalTexcoord(Vertex3D& v)
{
	return (this->u == v.u && this->v == v.v);
}