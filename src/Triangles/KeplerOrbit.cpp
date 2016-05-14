#include "KeplerOrbit.h"
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <algorithm>
#include <glm/gtc/matrix_transform.hpp>

KeplerOrbit::KeplerOrbit(GLuint shader_program, int line_steps, double ecc, double big_ha, double acc_node_long,
						 double inclin, double p_arg, glm::dvec3 base_position, double period) {
	this->ecc = ecc;
	this->big_ha = big_ha;
	this->small_ha = big_ha * glm::sqrt(1 - ecc * ecc);
	this->line_steps = line_steps;
	this->shader_program = shader_program;
	this->color = glm::vec3(1, 1, 1);
this->scale = 1;
	this->focal_dist = big_ha * ecc;
	this->acc_node_long = acc_node_long;
	this->p_arg = p_arg;
	this->inclin = inclin;
	this->period = period;
	updateTransform();
	
	glm::dvec4 pos = glm::inverse(transform) * glm::dvec4(base_position,1);

	pos.x /= big_ha;
	pos.z /= small_ha;

	this->cur_angle = atan2(pos.z, pos.x);

	orbit_line = new glm::vec3[line_steps];
	for (int i = 0; i < line_steps; ++i) {
		double angle = double(i) * 2 * glm::pi<double>() / line_steps;
		orbit_line[i].x = glm::cos(angle) * big_ha;
		orbit_line[i].z = glm::sin(angle) * small_ha;
		orbit_line[i].y = 0;
	}
	initGLBuffers("pos");
}

void KeplerOrbit::updateTransform() {
	transform = glm::scale(glm::dmat4x4(), scale, scale, scale);
	transform = glm::rotate(transform, acc_node_long - 270, glm::dvec3(0, 1, 0));
	transform = glm::rotate(transform, inclin, glm::dvec3(0, 0, 1));
	transform = glm::rotate(transform, p_arg - 90, glm::dvec3(0, 1, 0));
	transform = glm::translate(transform, -focal_dist, 0.0, 0.0);
}

double KeplerOrbit::calcSquare(double base_angle, double new_angle) {
	glm::vec2 a, b, c;
	a.x = focal_dist;
	a.y = 0;
	b.x = glm::cos(base_angle) * big_ha;
	b.y = glm::sin(base_angle) * small_ha;
	c.x = glm::cos(new_angle) * big_ha;
	c.y = glm::sin(new_angle) * small_ha;
	double delta = new_angle - base_angle;
	double seg_sq = 0.5 * (delta - glm::sin(delta)) * small_ha * big_ha;
	double la, lb, lc, p;
	la = glm::distance(a, b);
	lb = glm::distance(a, c);
	lc = glm::distance(c, b);
	p = (la + lb + lc) / 2;
	return glm::sqrt(abs(p * (p - la) * (p - lb) * (p - lc))) + seg_sq; 
}

double KeplerOrbit::find_new_angle(double sq_to_pass, double base_angle) {
	double cur_angle = base_angle - 2 * glm::pi<double>();
	double change = glm::pi<double>();
	double cur_square = calcSquare(cur_angle, base_angle);
	while (abs(sq_to_pass - cur_square) > 0.01 && change > 10E-10) {
		if (sq_to_pass > cur_square) {
			cur_angle -= change;
		} else {
			cur_angle += change;
		}
		cur_square = calcSquare(cur_angle, base_angle);
		change /= 2;
	}
	return cur_angle;
}

void KeplerOrbit::update(double dt) {
	while (cur_angle < -glm::pi<double>() * 2)
		cur_angle += glm::pi<double>() * 2;
	double square_to_pass = glm::pi<double>() * big_ha * small_ha * dt / period;
	this->cur_angle = find_new_angle(square_to_pass, cur_angle);
}

glm::vec3 KeplerOrbit::getPosition() {
	glm::dvec4 pos = glm::dvec4(glm::cos(cur_angle) * big_ha, 0, glm::sin(cur_angle) * small_ha, 1);
	pos = transform * pos;
	return glm::vec3(pos.x, pos.y, pos.z);
}

void KeplerOrbit::initGLBuffers(const char* posName) {
	glGenVertexArrays(1, &vao);
	glBindVertexArray (vao);

	glGenBuffers (1, &vbo);
		
	glBindBuffer (GL_ARRAY_BUFFER, vbo);
	glBufferData (GL_ARRAY_BUFFER, line_steps*sizeof(glm::vec3), orbit_line, GL_STATIC_DRAW);
	
	int	loc = glGetAttribLocation(shader_program, posName);
	if (loc>-1)
	{
		glVertexAttribPointer(loc,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(GLuint *)0);
		glEnableVertexAttribArray (loc);
	}
}

void KeplerOrbit::Draw(const BaseCamera &cam) const
{
	glm::mat4x4 modelViewMatrix = cam.ViewMatrix() * glm::mat4x4(transform);

	//finally calculate modelViewProjectionMatrix
	glm::mat4x4 modelViewProjectionMatrix = cam.ProjectionMatrix() * modelViewMatrix;
  
	//pass variables to the shaders
	int loc = glGetUniformLocation(shader_program, "modelViewProjectionMatrix");
	if (loc>-1)
		glUniformMatrix4fv(loc,1,0,glm::value_ptr(modelViewProjectionMatrix));
	if (loc < 0) return;
	loc = glGetUniformLocation(shader_program, "color");
	if (loc>-1)
		glUniform3f(loc, color.r, color.g, color.b);
	if (loc < 0) return;
	glBindVertexArray (vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glDrawArrays(GL_LINE_LOOP, 0, line_steps);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
}

KeplerOrbit::~KeplerOrbit() {
	delete[] orbit_line;
}