#pragma once
#include "BaseCamera.h"
#include "Sphere.h"

class SphereCamera : public BaseCamera 
{
	float zoom_acc;
	float zoom_max_speed;
	float acc;
	float max_speed;
	glm::vec3 cur_speed;
	double min_limit;
	double max_limit;
	Sphere *tie_center;
public:
	SphereCamera(glm::vec3 pos = glm::vec3(0, 0, 2000), glm::vec3 center = glm::vec3(),
		double zoom_acc = 3000.0, double zoom_max_speed = 1500.0, double acc = 4.0, double max_speed = 2.0);
	CamType getType();
	glm::mat4x4 ViewMatrix() const;
	glm::vec3 getPosition() const;
	bool isTied();
	void tieTo(Sphere *center);
	void stop();
	void setSpeedAcc(double max_speed, double acc);
	void setPosition(glm::vec3 pos);
	void setRadius(double rad);
	void setLimits(double min, double max);
	void update(float dt);
	static const int UP = 0;
	static const int DOWN = 1;
	static const int LEFT = 2;
	static const int RIGHT = 3;
	static const int ZOOM_IN = 4;
	static const int ZOOM_OUT = 5;
};