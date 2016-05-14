#pragma once
#include "BaseCamera.h"

class FreeCamera : public BaseCamera 
{
	float acc;
	float max_speed;
	glm::vec3 cur_speed;
	double min_limit;
	double max_limit;
public:
	FreeCamera(glm::vec3 pos = glm::vec3(0, 0, 2000), glm::vec3 center = glm::vec3(),
		float acc = 4000.0, float max_speed = 2000.0);
	CamType getType();
	void update(float dt);
	void setLimits(double min, double max);
	void stop();
	void setSpeedAcc(double max_speed, double acc);
	glm::vec3 getLookAt() const;
	void setLookAt(glm::vec3 pos);
	glm::mat4x4 ViewMatrix() const;
	static const int FORWARD = 0;
	static const int BACK = 1;
	static const int LEFT = 2;
	static const int RIGHT = 3;
};