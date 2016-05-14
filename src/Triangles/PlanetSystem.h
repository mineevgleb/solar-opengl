#pragma once
#include <glm/glm.hpp>
#include <ctime>
#include <vector>
#include "Sphere.h"

#define G (6.67259E-20)

struct SpaceObject {
	Sphere *s;
	double mass;
	double radius;
	double period;
	glm::dvec3 pos;
	glm::dvec3 speed;
};

class PlanetSystem 
{
	std::time_t time;
	std::vector<SpaceObject> obj1;
	std::vector<SpaceObject> obj2;
	std::vector<SpaceObject> *objects;
	std::vector<SpaceObject> *objects_prev;
	double time_step;
	double scale;
public:
	PlanetSystem();
	void addObject(SpaceObject obj);
	glm::vec3 getPosition(unsigned obj_num);
	void shortUpdate(double deltatime);
	void update(double deltatime);
};