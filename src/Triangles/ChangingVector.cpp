#include "ChangingVector.h"

ChangingVector::ChangingVector(glm::vec3 pos, double max_s, double a, double d) : 
	state(FREE), move_vector(), v(pos), dir(), max_speed(max_s), acc(a), dec(d) {};

void ChangingVector::moveTo(glm::vec3 pos, double time) {
	time_passed = 0;
	move_time = time;
	move_beg = v;
	move_end = pos;
	state = AIMED;
}

void ChangingVector::update(double dt) {
	if (state == FREE) {
		if(glm::length(dir) > 0.01) {
			glm::vec3 t = dir;
			t *= (acc + dec) * dt;
			move_vector += t;
		}
		double len = glm::length(move_vector);
		if (len) {
			move_vector /= len;
			len -= dec * dt;
			len = glm::clamp(len, 0.0, max_speed);
			move_vector *= len;
		}
		glm::vec3 cur_move = move_vector;
		cur_move *= dt;
		v += cur_move;
	}
	if (state == AIMED) {
		time_passed += dt;
		if (time_passed > move_time) {
			v = move_end;
			state = FREE;
			return;
		}
		double pos_in_time = (time_passed - move_time / 2) / (move_time / 2);
		pos_in_time = glm::sign(pos_in_time) * glm::sqrt(glm::abs(pos_in_time));
		pos_in_time = (pos_in_time + 1) / 2;
		glm::vec3 beg_part = move_beg;
		beg_part *= (1 - pos_in_time);
		glm::vec3 end_part = move_end;
		end_part *= pos_in_time;
		v = beg_part + end_part;
	}
}