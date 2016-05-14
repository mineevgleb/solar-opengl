#pragma once
#include "Ring.h"
#include "Sphere.h"

class RingSet {
	int rings_am;
	Ring **rings;
	double *flash_periods;
	double *cur_flash;
	Sphere *center;
public:
	RingSet(Sphere *center, int rings_am, int shader_prog, int tex);
	void Draw(const BaseCamera &cam, Light light) const;
	void update(double dt);
	~RingSet();
};