#include <glm/glm.hpp>
#include <GL/glew.h>
#include <GL/glut.h>
#include "BaseCamera.h"

class KeplerOrbit 
{
	double ecc;
	double big_ha;
	double small_ha;
	double focal_dist;
	double acc_node_long;
	double inclin;
	double p_arg;
	double cur_angle;
	double period;
	int line_steps;
	glm::vec3 *orbit_line;
	unsigned int vao;
	unsigned int vbo; 
	GLuint shader_program;
	glm::dmat4x4 transform;
	double calcSquare(double base_angle, double new_angle);
	double find_new_angle(double sq_to_pass, double base_angle);
public:
	glm::vec3 color;
	double scale;
	KeplerOrbit(GLuint shader_program, int line_steps, double ecc, double big_ha, double acc_node_long, double inclin, double p_arg,
		glm::dvec3 base_position, double period);
	glm::vec3 getPosition();
	void initGLBuffers(const char* posName);
	void updateTransform();
	void update(double dt);
	void KeplerOrbit::Draw(const BaseCamera &cam) const;
	~KeplerOrbit();
};