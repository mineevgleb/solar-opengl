#pragma once
#include "BaseObject.h"

class Ring : public BaseObject
{
	unsigned int steps;
public:
	double radius1;
	double radius2;
	Ring(Material m, double r1, double r2, unsigned int steps) : radius1(r1), radius2(r2), steps(steps), BaseObject(m)
	{
		generateData();
	}
private:
	void generateData();
};