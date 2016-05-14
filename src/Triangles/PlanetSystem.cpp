#include "PlanetSystem.h"
#include <cmath>

double distance(const SpaceObject &a, const SpaceObject &b) {
	return glm::distance(a.pos, b.pos);
}

PlanetSystem::PlanetSystem() : time_step(100), scale(100000) 
{
	objects = &obj1;
	objects_prev = &obj2;
}

void PlanetSystem::addObject(SpaceObject obj) {
	obj1.push_back(obj);
	obj2.push_back(obj);
}

glm::vec3 PlanetSystem::getPosition(unsigned obj_num) {
	glm::vec3 position = glm::vec3((*objects)[obj_num].pos);
	position /= scale;
	return position;
}

void PlanetSystem::shortUpdate(double deltatime) {
	static glm::dvec3 force, dir, acc, speed_delta, pos_delta1, pos_delta2;
	int size = objects->size();
	for (unsigned int i = 0; i < size; i++) {
		force.x = force.y = force.z = 0;
		for (unsigned int j = 0; j < size; j++) {
			if (i != j) {
				double dist = distance((*objects)[i], (*objects)[j]);
				double force_abs = G * (*objects)[i].mass * (*objects)[j].mass / (dist * dist);
				dir = (*objects)[j].pos - (*objects)[i].pos;
				dir /= dist;
				dir *= force_abs;
				force += dir;
			}
		}
		acc = force;
		acc /= (*objects)[i].mass;
		speed_delta = acc;
		speed_delta *= deltatime;
		(*objects_prev)[i].speed = (*objects)[i].speed + speed_delta;
		pos_delta1 = (*objects)[i].speed;
		pos_delta1 *= deltatime;
		pos_delta2 = acc;
		pos_delta2 *= deltatime * deltatime / 2;
		(*objects_prev)[i].pos = (*objects)[i].pos + pos_delta1 + pos_delta2;
	}
	std::vector<SpaceObject> *t;
	t = objects_prev;
	objects_prev = objects;
	objects = objects_prev;
}

void PlanetSystem::update(double deltatime) {
	while(deltatime > time_step) {
		shortUpdate(time_step);
		deltatime -= time_step;
	}
	shortUpdate(deltatime);
}