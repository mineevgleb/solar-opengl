#include "RingSet.h"
#include "cmath"

RingSet::RingSet(Sphere *center, int rings_am, int shader_prog, int tex) : 
	rings_am(rings_am), center(center) {
	double radius1 = center->radius * 1.2; 
	double radius2 = center->radius * 2; 
	rings = new Ring*[rings_am];
	flash_periods = new double[rings_am];
	cur_flash = new double[rings_am];
 	for (int i = 0; i <rings_am; i++) {
		Material m(shader_prog, tex);
		m.specular = 0.1;
		m.diffuse = 0.5;
		m.ambient = 1.0;
		float r1 = radius1 + (radius2 - radius1) / rings_am * i; 
		float r2 = radius1 + (radius2 - radius1) / rings_am * (i + 1); 
		rings[i] = new Ring(m, r1, r2, 500);
		rings[i]->rotation.z = center->rotation.z;
		rings[i]->rotation.y = double(rand() % 360);
		flash_periods[i] = 24 * 60 * 60 * 30 + (double(rand() % 10000) / 10000) * 24 * 60 * 60 * 30; 
		cur_flash[i] = double(rand() % 100) / 100 * 3.14;
	}
}

void RingSet::update(double dt) {
	for (int i = 0; i <rings_am; i++) {
		rings[i]->position = center->position;
		rings[i]->scale = center->scale;
		rings[i]->obj_material.alpha = 0.5 + 0.5 * glm::sin(cur_flash[i]);
		cur_flash[i] += dt / flash_periods[i];
		rings[i]->rotation.y += dt * (double(rings_am - i) / rings_am) / (60 * 60);
		while (rings[i]->rotation.y > 360) {
			rings[i]->rotation.y -= 360;
		}
	}
}

void RingSet::Draw(const BaseCamera &cam, Light light) const {
	for (int i = 0; i <rings_am; i++) {
		rings[i]->Draw(cam, light);
	}
}

RingSet::~RingSet() {
	for (int i = 0; i <rings_am; i++)
		delete rings[i];
	delete[] rings;
	delete[] flash_periods;
	delete[] cur_flash;
}