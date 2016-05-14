#include "ChangingValue.h"
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

ChangingValue::ChangingValue() : value(1.0) {}

void ChangingValue::changeTo(double val, double time) {
	base_val = value;
	dest_val = val;
	ch_time = time;
	cur_time = 0;
}

void ChangingValue::update(double dt) {
	if(ch_time > 0) {
		cur_time += dt;
		if (cur_time > ch_time) {
			value = dest_val;
			ch_time = -1;
			return;
		}
		double pos_in_time = (cur_time - ch_time / 2) / (ch_time / 2);
		pos_in_time = glm::atan(pos_in_time) * 4 / glm::pi<float>();
		pos_in_time = (pos_in_time + 1.0f) / 2;
		value = base_val * (1 - pos_in_time) + dest_val * pos_in_time;
	}
}