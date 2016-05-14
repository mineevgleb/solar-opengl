#include "FreeCamera.h"
#include <glm/gtc/matrix_transform.hpp>

FreeCamera::FreeCamera(glm::vec3 pos, glm::vec3 center, float acc, float max_speed) :
	BaseCamera(pos, center, 4), acc(acc), max_speed(max_speed) {
	keys[FORWARD] = 'w';
	keys[BACK] = 's';
	keys[LEFT] = 'a';
	keys[RIGHT] = 'd';
	setLookAt(center);
	min_limit = 0;
	max_limit = 100000000000;
}

void FreeCamera::setSpeedAcc(double max_speed, double acc) {
	this->max_speed = max_speed;
	this->acc = acc;
}

glm::mat4x4 FreeCamera::ViewMatrix() const {
	glm::vec3 up = glm::vec3(0, 1, 0);
	return glm::lookAt(pos, getLookAt(), up);
}

glm::vec3 FreeCamera::getLookAt() const {
	glm::vec3 pos_decart;
	pos_decart.x = glm::cos(look_at.y) * glm::cos(look_at.x) * look_at.z;
	pos_decart.y = glm::sin(look_at.y) * look_at.z;
	pos_decart.z = glm::cos(look_at.y) * glm::sin(look_at.x) * look_at.z;
	return pos_decart + pos;
}

void FreeCamera::setLookAt(glm::vec3 pos) {
	pos -= this->pos;
	look_at.z = 1;
	pos = glm::normalize(pos);
	look_at.x = atan2f(pos.z, pos.x);
	look_at.y = glm::asin(pos.y);
}

void FreeCamera::stop() {
	cur_speed = glm::vec3();
	for (int i = 0; i < keys_am; i++)
		keys_down[i] = false;
}

CamType FreeCamera::getType() {
	return CamType::FREE;
}

void FreeCamera::setLimits(double min, double max) {
	min_limit = min;
	max_limit = max;
}

void FreeCamera::update(float dt) {
		if (is_mouse_down) {
			glm::vec2 delta = mouse_pos - click_pos;
			delta.x /= width;
			delta.y /= height;
			look_at.x += delta.x * 3;
			look_at.y -= delta.y * 3;
			click_pos = mouse_pos;
			look_at.y = glm::clamp(look_at.y, -1.4f, 1.4f);
		}
		glm::vec3 dir;
		if(keys_down[FORWARD]) {
			dir += getLookAt() - pos;
		}
		if(keys_down[BACK]) {
			dir -= getLookAt() - pos;
		}
		if(keys_down[LEFT]) {
			glm::vec3 left_vec = -glm::cross(getLookAt() - pos, glm::vec3(0,1,0));
			dir += glm::normalize(left_vec);
		}
		if(keys_down[RIGHT]) {
			glm::vec3 left_vec = -glm::cross(getLookAt() - pos, glm::vec3(0,1,0));
			dir -= glm::normalize(left_vec);
		}
		glm::vec3 t = dir;
		t *= 2 * acc * dt;
		cur_speed += t;
		float len = glm::length(cur_speed);
		if (len > 0.1)
			cur_speed /= len;
		len -= acc * dt;
		len = glm::clamp(len, 0.0f, max_speed);
		cur_speed *= len;
		glm::vec3 pos_inc = cur_speed;
		pos_inc *= dt;
		pos += pos_inc;		
		if (glm::length(pos) < min_limit) {
			pos /= glm::length(pos);
			pos *= min_limit;
		}
		if (glm::length(pos) > max_limit) {
			pos /= glm::length(pos);
			pos *= max_limit;
		}
}