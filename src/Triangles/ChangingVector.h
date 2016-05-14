#pragma once
#include <glm/glm.hpp>

class ChangingVector  {
	int state;
	const static int AIMED = 1; 
	const static int FREE = 2;
	
	double move_time;
	double time_passed;
	glm::vec3 move_beg;
	glm::vec3 move_end;
public:
	ChangingVector(glm::vec3 pos = glm::vec3(), double max_s = 2, double a = 8, double d = 4);
	glm::vec3 v;
	glm::vec3 dir;
	glm::vec3 move_vector;
	double max_speed;
	double acc;
	double dec;
	void moveTo(glm::vec3 pos, double time);
	void update(double dt);
};