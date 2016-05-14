#pragma once

class ChangingValue {
private:
	double dest_val;
	double base_val;
	double ch_time;
	double cur_time;
public:
	ChangingValue();
	double value;
	void changeTo(double val, double time);
	void update(double dt);
};