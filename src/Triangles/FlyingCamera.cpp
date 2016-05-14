#include "FlyingCamera.h"
#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>

FlyingCamera::FlyingCamera(void (*end_func)(), glm::vec3 pos, glm::vec3 center) : 
	BaseCamera(pos, center, 1), end_func(end_func) 
{
	setLookAt(center);
}

CamType FlyingCamera::getType() {
	return CamType::FLYING;
}

void FlyingCamera::updateDest(glm::vec3 dest_pos) {
	this->dest_pos = dest_pos;
}

void FlyingCamera::flyTo(glm::vec3 dest_pos, glm::vec3 dest_center, float time) {
	dest_look_len = glm::length(dest_pos - dest_center);
	this->dest_pos = dest_pos;
	this->base_pos = pos;
	pos = dest_pos;
	this->base_center = look_at;
	setLookAt(dest_center);
	this->dest_center = look_at;
	look_at = this->base_center;
	pos = base_pos;
	this->time = time;
	while (this->dest_center.x - this->base_center.x < -glm::pi<float>()) {
		this->dest_center.x += 2 * glm::pi<float>();
	}
	while (this->dest_center.x - this->base_center.x > glm::pi<float>()) {
		this->dest_center.x -= 2 * glm::pi<float>();
	}
	cur_time = 0;
}

glm::vec3 FlyingCamera::getLookAt() const {
	glm::vec3 pos_decart;
	pos_decart.x = glm::cos(look_at.y) * glm::cos(look_at.x) * look_at.z;
	pos_decart.y = glm::sin(look_at.y) * look_at.z;
	pos_decart.z = glm::cos(look_at.y) * glm::sin(look_at.x) * look_at.z;
	return pos_decart + pos;
}

void FlyingCamera::setLookAt(glm::vec3 pos) {
	pos -= this->pos;
	look_at.z = 1;
	pos = glm::normalize(pos);
	look_at.x = atan2f(pos.z, pos.x);
	look_at.y = glm::asin(pos.y);
}

glm::mat4x4 FlyingCamera::ViewMatrix() const {
	glm::vec3 up = glm::vec3(0, 1, 0);
	return glm::lookAt(pos, getLookAt(), up);
}

void FlyingCamera::update(float dt) {
	cur_time += dt;
	if (cur_time > time) {
		pos = dest_pos;
		look_at = dest_center;
		look_at.z = dest_look_len;
		end_func();
		return;
	}
	double pos_in_time = (cur_time - time / 2) / (time / 2);
	pos_in_time = glm::atan(pos_in_time) * 4 / glm::pi<float>();
	pos_in_time = (pos_in_time + 1.0f) / 2;
	glm::vec3 base_part = base_pos;
	base_part *= (1.0f - pos_in_time);
	glm::vec3 dest_part = dest_pos;
	dest_part *= pos_in_time;
	pos = base_part + dest_part;
	base_part = base_center;
	base_part *= (1.0f - pos_in_time);
	dest_part = dest_center;
	dest_part *= pos_in_time;
	look_at = base_part + dest_part;
}