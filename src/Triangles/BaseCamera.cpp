#include "BaseCamera.h"
#include <glm/gtc/matrix_transform.hpp>

BaseCamera::BaseCamera(glm::vec3 pos, glm::vec3 look_at, int keys_am) : 
	field_of_view(45.0f), z_near(0.001f), z_far(10000000.0f), pos(pos), 
	look_at(look_at), keys_am(keys_am), is_mouse_down(false)
{
	keys = new int[keys_am];
	keys_down = new bool[keys_am];
	for (int i = 0; i < keys_am; i++)
		keys_down[i] = false;
}  

BaseCamera::~BaseCamera() {
	delete[] keys;
	delete[] keys_down;
}

void BaseCamera::setActionKey(int action, int key) {
	keys[action] = key;
}

void BaseCamera::keyDown(int key) {
	for (int i = 0; i < keys_am; i++) {
		if (keys[i] == key) keys_down[i] = true;
	}
}

void BaseCamera::mouseClicked(int x, int y) {
	is_mouse_down = true;
	click_pos = glm::vec2(x, y);
	mouse_pos = glm::vec2(x, y);
}

void BaseCamera::mouseMoved(int x, int y) {
	mouse_pos = glm::vec2(x, y);
}

void BaseCamera::mouseReleased() {
	is_mouse_down = false;
}

void BaseCamera::keyUp(int key) {
	for (int i = 0; i < keys_am; i++) {
		if (keys[i] == key) keys_down[i] = false;
	}
}

void BaseCamera::setNearClipPlane(float p) {
	z_near = p;
	z_far = p * 10000000.0f;
}

void BaseCamera::setProjParams(float w, float h, double fov, double znear) {
	width = w;
	height = h;
	field_of_view = fov;
	z_near = znear;
	z_far = znear * 10000000.0f;
}

glm::mat4x4 BaseCamera::ProjectionMatrix() const {
	return glm::perspective(field_of_view, width / height, z_near, z_far);
}

glm::mat4x4 BaseCamera::ViewMatrix() const {
	glm::vec3 up = glm::vec3(0, 1, 0);
	return glm::lookAt(pos, look_at, up);
}

glm::vec3 BaseCamera::getPosition() const {
	return pos;
}

void BaseCamera::setPosition(glm::vec3 pos) {
	this->pos = pos;
}

glm::vec3 BaseCamera::getLookAt() const {
	return look_at;
}

void BaseCamera::setLookAt(glm::vec3 pos) {
	this->look_at = pos;
}