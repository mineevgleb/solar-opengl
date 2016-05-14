#pragma once
#include "BaseCamera.h"

class FlyingCamera : public BaseCamera 
{
	glm::vec3 base_pos;
	glm::vec3 dest_pos;
	glm::vec3 base_center;
	glm::vec3 dest_center;
	float dest_look_len;
	float time;
	float cur_time;
	void (*end_func)();
public:
	FlyingCamera(void (*end_func)(), glm::vec3 pos = glm::vec3(0, 0, 2000), glm::vec3 center = glm::vec3());
	CamType getType();
	glm::mat4x4 ViewMatrix() const;
	glm::vec3 getLookAt() const;
	void setLookAt(glm::vec3 pos);
	void flyTo(glm::vec3 dest_pos, glm::vec3 dest_center, float time); 
	void updateDest(glm::vec3 dest_pos);
	void update(float dt);
};