#pragma once
#include "BaseObject.h"

class Sphere : public BaseObject
{
	unsigned int iter_am;
public:
	double radius;
	Sphere(Material m, double r, unsigned int iterations) : radius(r), iter_am(iterations), BaseObject(m)
	{
		generateData();
	}
private:
	void tesselate(VertexData *&vert, unsigned int *&ind, unsigned int &vert_am, unsigned int &ind_am);
	void make_seam(VertexData *&vert, unsigned int *&ind, unsigned int &vert_am, unsigned int &ind_am);
	void generateData();
};