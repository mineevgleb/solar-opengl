#include "SphereCamera.h"
#include <glm/gtc/matrix_transform.hpp>

SphereCamera::SphereCamera(glm::vec3 pos, glm::vec3 center,
						   double zoom_acc, double zoom_max_speed, double acc, double max_speed) : 
BaseCamera(pos, center, 6), zoom_acc(zoom_acc), zoom_max_speed(zoom_max_speed), acc(acc), max_speed(max_speed) 
{
	keys[UP] = 'w';
	keys[DOWN] = 's';
	keys[LEFT] = 'a';
	keys[RIGHT] = 'd';
	keys[ZOOM_IN] = 'q';
	keys[ZOOM_OUT] = 'e';
	setPosition(pos);
	min_limit = 0;
	max_limit = 100000000000;
	tie_center = NULL;
}

CamType SphereCamera::getType() {
	return CamType::SPHERE;
}

void SphereCamera::tieTo(Sphere *center) {
	tie_center = center;
}

glm::mat4x4 SphereCamera::ViewMatrix() const {
	glm::vec3 up = glm::vec3(0, 1, 0);
	return glm::lookAt(getPosition(), look_at, up);
}

void SphereCamera::stop() {
	cur_speed = glm::vec3();
	for (int i = 0; i < keys_am; i++)
		keys_down[i] = false;
}

glm::vec3 SphereCamera::getPosition() const {
	glm::vec3 pos_decart;
	pos_decart.x = glm::cos(pos.y) * glm::cos(pos.x) * pos.z;
	pos_decart.y = glm::sin(pos.y) * pos.z;
	pos_decart.z = glm::cos(pos.y) * glm::sin(pos.x) * pos.z;
	return pos_decart + look_at;
}

void SphereCamera::setPosition(glm::vec3 pos) {
	pos -= look_at;
	this->pos.z = glm::length(pos);
	pos = glm::normalize(pos);
	this->pos.x = atan2f(pos.z, pos.x);
	this->pos.y = glm::asin(pos.y);
}

void SphereCamera::setSpeedAcc(double max_speed, double acc) {
	this->zoom_max_speed = max_speed;
	this->zoom_acc = acc;
}

void SphereCamera::setLimits(double min, double max) {
	min_limit = min;
	max_limit = max;
}

bool SphereCamera::isTied() {
	return tie_center != NULL;
}

void SphereCamera::setRadius(double rad) {
	pos.z = rad;
}

void SphereCamera::update(float dt) {
	if (tie_center) {
		this->look_at = tie_center->position;
		this->min_limit = tie_center->radius * tie_center->scale.x * 5;
		if (this->min_limit < tie_center->radius * tie_center->scale.x + 0.13)
			this->min_limit = tie_center->radius * tie_center->scale.x + 0.13;
	}
	if (is_mouse_down) {
		cur_speed = glm::vec3();
		glm::vec2 delta = mouse_pos - click_pos;
		delta.x /= width;
		delta.y /= height;
		pos.x += delta.x * 3;
		pos.y += delta.y * 3;
		click_pos = mouse_pos;
		pos.y = glm::clamp(pos.y, -1.4f, 1.4f);
		return;
	}
	glm::vec3 dir;
	if(keys_down[UP]) {
		dir.y -= 1;
	}
	if(keys_down[DOWN]) {
		dir.y += 1;
	}
	if(keys_down[LEFT]) {
		dir.x -= 1;
	}
	if(keys_down[RIGHT]) {
		dir.x += 1;
	}
	if(keys_down[ZOOM_IN]) {
		dir.z += 1;
	}
	if(keys_down[ZOOM_OUT]) {
		dir.z -= 1;
	}
	int a = glm::sign(0);
	double prev_speed;
	prev_speed = cur_speed.z;
	cur_speed.z += dir.z * 2 * zoom_acc * dt - zoom_acc * dt * glm::sign(cur_speed.z);
	if (glm::sign(prev_speed) == -glm::sign(cur_speed.z)) cur_speed.z = 0; 
	cur_speed.z = glm::clamp(cur_speed.z, -zoom_max_speed, zoom_max_speed);

	prev_speed = cur_speed.x;
	cur_speed.x += dir.x * 2 * acc * dt - acc * dt * glm::sign(cur_speed.x);
	if (glm::sign(prev_speed) == -glm::sign(cur_speed.x)) cur_speed.x = 0; 
	cur_speed.x = glm::clamp(cur_speed.x, -max_speed, max_speed);

	prev_speed = cur_speed.y;
	cur_speed.y += dir.y * 2 * acc * dt - acc * dt * glm::sign(cur_speed.y);
	if (glm::sign(prev_speed) == -glm::sign(cur_speed.y)) cur_speed.y = 0; 
	cur_speed.y = glm::clamp(cur_speed.y, -max_speed, max_speed);
	glm::vec3 pos_inc = cur_speed;
	pos_inc *= dt;
	pos += pos_inc;
	if (glm::abs(pos.y) > 1.4f) {
		pos.y = glm::clamp(pos.y, -1.4f, 1.4f);
		cur_speed.y = 0;
	}
	if (pos.z < min_limit) pos.z = min_limit;
	if (pos.z > max_limit) pos.z = max_limit;
		
}