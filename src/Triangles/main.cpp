//standard libraries
#include <iostream>
#include <fstream>
#include <vector>
using namespace std;
//opengl headers
#include <GL/glew.h>
#include <GL/glut.h>

//opengl mathematics
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

#include <chrono>

//functions for shader compilation and linking
#include "shaderhelper.h"
//object for drawing
#include "BaseObject.h"
#include "Sphere.h"
#include "Ring.h"
#include "PlanetSystem.h"
#include "KeplerOrbit.h"
#include "SphereCamera.h"
#include "FreeCamera.h"
#include "FlyingCamera.h"
#include "ChangingValue.h"
#include "RingSet.h"

//library to load textures
#define LODEPNG_COMPILE_ERROR_TEXT
#include "../lodepng/lodepng.h"

#define BASE_SCALER 0.00001

int obj_am;
KeplerOrbit **orbits;
Sphere **objects;
double *rot_periods;

double schem_sun_scaler;
double schem_planets_scaler;
ChangingValue cur_sun_scaler;
ChangingValue cur_planets_scaler;

double *schem_orbits_sclaers;
ChangingValue *cur_orbits_scalers;
const double tran_time = 0.7; 
double cur_tran_time = -1;

BaseCamera* cam;
SphereCamera* sphere_cam;
FreeCamera* free_cam;
FlyingCamera* fly_cam;

//struct for loading shaders
ShaderProgram shaderProgram;
ShaderProgram lineShaderProgram;

//window size
int windowWidth = 800;
int windowHeight = 600;

double time_scaler = 24 * 60 * 60 * 2;
bool inc_scaler = false;
bool dec_scaler = false; 
bool stop = false;

//matrices
glm::mat4x4 modelViewMatrix;
glm::mat4x4 projectionMatrix;
glm::mat4x4 modelViewProjectionMatrix;
glm::mat4x4 normalMatrix;

int mousex = 0, mousey = 0;

bool is_scheme = false;

//texture identificator
GLuint *texId;

RingSet * saturn_rings;

//names of shader files. program will search for them during execution
//don't forget place it near executable 
char VertexShaderName[] = "Vertex.vert";
char FragmentShaderName[] = "Fragment.frag";

char LineVertexShaderName[] = "LineVertex.vert";
char LineFragmentShaderName[] = "LineFragment.frag";

////////////////////////////////////////////////////////
///
void initTextures(unsigned amount, string *tex_names, GLuint *texIds)
{
	//generate as many textures as you need
	glGenTextures(amount, texIds);
	vector<unsigned char> texture;
	unsigned width, height;

	for (unsigned int i = 0; i < amount; i++) { 
		glBindTexture(GL_TEXTURE_2D, texId[i]);
	
		glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	
		// Set nearest filtering mode for texture minification
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		
		texture.clear();
		lodepng::decode(texture, width, height, tex_names[i]);
		//set Texture Data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &texture[0]);
	}
}

void change_to_spherecam() {
	sphere_cam->setLookAt(cam->getLookAt());
	sphere_cam->setPosition(cam->getPosition());
	cam = sphere_cam;
}

/////////////////////////////////////////////////////////////////////
///is called when program starts
void init()
{
	
	//initialize shader program
	shaderProgram.init(VertexShaderName,FragmentShaderName);
	lineShaderProgram.init(LineVertexShaderName, LineFragmentShaderName);
	//use this shader program
	
	ifstream in("objects.txt");
	glm::dvec3 offset;
	in >> obj_am;
	texId = new GLuint[obj_am + 1];
	orbits = new KeplerOrbit* [obj_am];
	objects = new Sphere* [obj_am];
	rot_periods = new double[obj_am - 1];
	schem_orbits_sclaers = new double[obj_am - 2];
	cur_orbits_scalers = new ChangingValue[obj_am - 2];
	string *tex_names = new string[obj_am + 1];
	for (int i = 0; i < obj_am + 1; ++i) {
		in >> tex_names[i];
	}
	initTextures(obj_am + 1, tex_names, texId);
	delete[] tex_names;

	double radius, big_ha, ecc, acc_node_l, p_arg, inclin, period;
	glm::dvec3 pos;
	for (int i = 0; i < obj_am; ++i) {
		Material m(shaderProgram.programObject, texId[i]);
		if (i <= 1) {
			m.ambient = 1.0f;
			m.diffuse = 0.0;
			m.specular = 0.0;
			m.gloss = 0.0;
		} else {
			m.ambient = 0.05;
			m.diffuse = 0.6;
			m.specular = 0.1;
			m.gloss = 2;
		}
		in >> radius;
		radius *= BASE_SCALER;
		objects[i]= new Sphere(m, radius, 3);
		if (i >= 2) {
			in >> big_ha >> ecc >> acc_node_l >> p_arg >> inclin >> period;
			period *= 24 * 60 * 60;
		}
		if (i >= 1) {
			in >> rot_periods[i] >> objects[i]->rotation.z;
			rot_periods[i] *= 24 * 60 * 60;
		}
		if (i >= 2) {
			in >> pos.x >> pos.z >> pos.y;
			big_ha *= BASE_SCALER;
			pos.z *= -1;
			pos *= BASE_SCALER;
			orbits[i - 2] = new KeplerOrbit(lineShaderProgram.programObject, 2000 * (i - 1), ecc, big_ha, acc_node_l, inclin, p_arg, pos, period);
		}
		
	}
	in >> schem_sun_scaler >> schem_planets_scaler;
	for(int i = 0; i < obj_am - 2; i++) 
		in >> schem_orbits_sclaers[i];
	sphere_cam = new SphereCamera();
	sphere_cam->setLimits(0, 74500);
	free_cam = new FreeCamera();
	free_cam->setLimits(0, 74500);
	fly_cam = new FlyingCamera(change_to_spherecam);
	cam = sphere_cam;
	glEnable(GL_DEPTH_TEST);
	glLineWidth(1);
	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	saturn_rings = new RingSet(objects[7], 50, shaderProgram.programObject, texId[obj_am]);
}


/////////////////////////////////////////////////////////////////////
///called when window size is changed
void reshape(int width, int height)
{
  windowWidth = width;
  windowHeight = height;
  //set viewport to match window size
  glViewport(0, 0, width, height);

  free_cam->setProjParams(float(width), float(height), 45.0, 0.1);
  fly_cam->setProjParams(float(width), float(height), 45.0, 0.1);
  sphere_cam->setProjParams(float(width), float(height), 45.0, 0.1);
}

////////////////////////////////////////////////////////////////////
///actions for single frame
void display()
{
  glClearColor(0,0,0,0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

  Light l;
  l.position = glm::vec3(0, 0, 0);
  l.intensity = glm::vec3(1.0f, 0.9f, 0.8f);
  glUseProgram(lineShaderProgram.programObject);
  for (int i = 0; i < obj_am - 2; i++)
	  orbits[i]->Draw(*cam);
  glUseProgram(shaderProgram.programObject);
  objects[0]->Draw(*cam, l);
  for (int i = 1; i < obj_am; i++) {
	glEnable(GL_STENCIL_TEST);
	glStencilFunc(GL_ALWAYS, i, 0);
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE); 
	objects[i]->Draw(*cam, l);
	glDisable(GL_STENCIL_TEST);
  }
  saturn_rings->Draw(*cam, l);
  
  //end frame visualization
  glutSwapBuffers();
  
}

double dist_to_nearest_planet(int *planet_index) {
	double min = -1;
	for (int i = 1; i < obj_am; i++) {
		double cur = glm::distance(cam->getPosition(), objects[i]->position);
		if (min < 0 || cur < min) {
			min = cur;
			*planet_index = i;
		}
	}
	return min;
}

//////////////////////////////////////////////////////////////////////////
///IdleFunction
void update()
{
	glm::vec4 a = glm::vec4(0,0,0,1);
	a = cam->ViewMatrix() * cam->ProjectionMatrix() * a;
	static chrono::system_clock::time_point prev = chrono::system_clock::now();
	chrono::system_clock::time_point cur = std::chrono::system_clock::now();
	//make animation
	double dt = double((cur - prev).count()) * 
		chrono::system_clock::period::num / chrono::system_clock::period::den;

	cur_planets_scaler.update(dt);
	cur_sun_scaler.update(dt);
	for(int i = 0; i < obj_am - 2; i++)
		cur_orbits_scalers[i].update(dt);

	objects[1]->scale.x = objects[1]->scale.y = objects[1]->scale.z = cur_sun_scaler.value;
	for (int i = 2; i < obj_am; i++) {
		objects[i]->position = orbits[i - 2]->getPosition();
		objects[i]->scale.x = objects[i]->scale.y = objects[i]->scale.z = cur_planets_scaler.value;
		orbits[i - 2]->scale = cur_orbits_scalers[i - 2].value;
		orbits[i - 2]->updateTransform();
	}
	if (!stop)
	for (int i = 1; i < obj_am; i++) {
		objects[i]->rotation.y += dt * time_scaler * 360 / rot_periods[i];
		while (objects[i]->rotation.y > 360)
			objects[i]->rotation.y -= 360;
		saturn_rings->update(dt * time_scaler);
	}
	if (!stop) {
		if (inc_scaler) {
			time_scaler += (time_scaler) * dt;
			if (time_scaler > 60 * 60 * 24 * 365 * 10) time_scaler = 60 * 60 * 24 * 365 * 10;
		}
		if (dec_scaler) {
			time_scaler -= (time_scaler) * dt;
			if (time_scaler < 1) time_scaler = 1;
		}
	}
	if (cam == sphere_cam || cam == free_cam) {
		int plan_num;
		double dist = dist_to_nearest_planet(&plan_num);
		dist = glm::max(dist, 1.0);
		glm::vec3 cam_pos = cam->getPosition();
		glm::vec3 plan_pos = objects[plan_num]->position;
		double planet_rad = objects[plan_num]->radius * objects[plan_num]->scale.x;
		double rad_scaler = 3.2;
		if (is_scheme) rad_scaler = 1.4;
		if (glm::distance(cam_pos, plan_pos) < planet_rad * rad_scaler) {
			cam_pos -= plan_pos;
			cam_pos /= glm::length(cam_pos);
			cam_pos *= planet_rad * rad_scaler;
			cam_pos += plan_pos;
			cam->setPosition(cam_pos);
		}
		sphere_cam->setSpeedAcc(dist, dist * 4);
		free_cam->setSpeedAcc(dist, dist * 4);
	}
	if (cam == fly_cam && sphere_cam->isTied()) {
		sphere_cam->update(dt);
		fly_cam->updateDest(sphere_cam->getPosition());
	}
	cam->update(dt);
	objects[0]->position = cam->getPosition();
	if (!stop)
	for (int i = 0; i < obj_am - 2; i++) {
		orbits[i]->update(time_scaler * dt);
	}
	prev = cur;
	if (cur_tran_time > 0)
		cur_tran_time += dt;
	if (cur_tran_time > tran_time && cur_tran_time > 0) {
		cur_tran_time = -1;
		sphere_cam->setNearClipPlane(0.1);
		free_cam->setNearClipPlane(0.1);
		fly_cam->setNearClipPlane(0.1);
	}

	static int active = -1;
	GLbyte val;
	glReadPixels(mousex, glutGet(GLUT_WINDOW_HEIGHT) - mousey, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &val);
	if (active >= 0) {
		orbits[active]->color = glm::vec3(1, 1, 1);
		active = -1;
	}
	if (val > 1) {
		orbits[val - 2]->color = glm::vec3(0, 0.5, 1);
		active = val - 2;
	}

	glutPostRedisplay();
}


unsigned char to_lower_eng(unsigned char a) {
	std::string eng = "qwertyuiopasdfghjklzxcvbnm";
	std::string rus = "יצףךוםדרשחפûגאןנמכהÿקסלטעü";
	a = tolower(a);
	if (!(a >= 'a' && a <= 'z')) {
		for (int i = 0; i < rus.length(); i++) {
			if (a == static_cast<unsigned char>(rus[i])) return eng[i];
		}
	} else {
		return a;
	}
	return 0;
}

/////////////////////////////////////////////////////////////////////////
///is called when key on keyboard is pressed
///use SPACE to switch mode
///TODO: place camera transitions in this function
void keyboardDown(unsigned char key, int mx, int my)
{
	cam->keyDown(to_lower_eng(key));
	if (key == '+' || key == '=') { 
		inc_scaler = true;
	}
	if (key == '-' || key == '_') { 
		dec_scaler = true;
	}
}

void keyboardUp(unsigned char key, int mx, int my)
{
	cam->keyUp(to_lower_eng(key));
	if (key == '+' || key == '=') { 
		inc_scaler = false;
	}
	if (key == '-' || key == '_') { 
		dec_scaler = false;
	}
	if (key == ' ') {
		stop = !stop;
	}
	if (key >= '0' && key <= '9') {
		fly_cam->setPosition(cam->getPosition());
		fly_cam->setLookAt(cam->getLookAt());
		sphere_cam->tieTo(objects[key - '0' + 1]);
		sphere_cam->update(0);
		fly_cam->flyTo(sphere_cam->getPosition(), sphere_cam->getLookAt(), 2);
		cam = fly_cam;
		if (key == '0')
			sphere_cam->setLimits(0, 74500);
		else
			sphere_cam->setLimits(0, 20000);
	}
}

void specialUp(int key, int mx, int my)
{
	const int shift_key = 112;
	const int ctrl_key = 114;
	if (key == shift_key) {
		if (cam->getType() == CamType::SPHERE) {
			sphere_cam->tieTo(NULL);
			free_cam->setPosition(cam->getPosition());
			free_cam->setLookAt(cam->getLookAt());
			sphere_cam->stop();
			cam = free_cam;
		}
		else if (cam->getType() == CamType::FREE) {
			sphere_cam->setLimits(0, 74500);
			fly_cam->setPosition(cam->getPosition());
			fly_cam->setLookAt(cam->getLookAt());
			fly_cam->flyTo(cam->getPosition(), glm::vec3(0,0,0), 1.0);
			free_cam->stop();
			cam = fly_cam;
		}
	}
	if (key == ctrl_key) {
		if (!is_scheme) {
			cur_sun_scaler.changeTo(schem_sun_scaler, tran_time);
			cur_planets_scaler.changeTo(schem_planets_scaler, tran_time);
			for (int i = 0; i < obj_am - 2; ++i)
				cur_orbits_scalers[i].changeTo(schem_orbits_sclaers[i], tran_time);
			is_scheme = true; 
			sphere_cam->setNearClipPlane(10);
			free_cam->setNearClipPlane(10);
			fly_cam->setNearClipPlane(10);
		} else {
			cur_sun_scaler.changeTo(1, tran_time);
			cur_planets_scaler.changeTo(1, tran_time);
			for (int i = 0; i < obj_am - 2; ++i)
				cur_orbits_scalers[i].changeTo(1, tran_time);
			is_scheme = false;
			cur_tran_time = 0.0000001;
		}
	}
}

/////////////////////////////////////////////////////////////////////////
///is called when mouse button is pressed
///TODO: place camera rotations in this function
void mouse(int button, int mode,int posx, int posy)
{
	if (button == GLUT_LEFT_BUTTON)
	{
		if (mode == GLUT_DOWN)
		{
			cam->mouseClicked(posx, posy);
		}
		else
		{
			cam->mouseReleased();
			GLbyte val;
			glReadPixels(posx, glutGet(GLUT_WINDOW_HEIGHT) - posy, 1, 1, GL_STENCIL_INDEX, GL_UNSIGNED_BYTE, &val);
			if (val > 0) {
				fly_cam->setPosition(cam->getPosition());
				fly_cam->setLookAt(cam->getLookAt());
				sphere_cam->tieTo(objects[val]);
				sphere_cam->update(0);
				sphere_cam->setRadius(objects[val]->radius * 10);
				fly_cam->flyTo(sphere_cam->getPosition(), sphere_cam->getLookAt(), 2);
				cam = fly_cam;
				if (val == 1)
					sphere_cam->setLimits(0, 74500);
				else
					sphere_cam->setLimits(0, 20000);
			}
		}
	}
}

void mousemove(int posx, int posy)
{
	cam->mouseMoved(posx, posy);
}

void passive_mousemove(int posx, int posy)
{
	mousex = posx;
	mousey = posy;
}

////////////////////////////////////////////////////////////////////////
///this function is used in case of InitializationError
void emptydisplay()
{
}

////////////////////////////////////////////////////////////////////////
///entry point
int main (int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
	glutCreateWindow("Solar System");

	glewInit();
	const char * slVer = (const char *) glGetString ( GL_SHADING_LANGUAGE_VERSION );
	cout << "GLSL Version: " << slVer << endl;

	try
	{
		init();
		glutDisplayFunc(display);
		glutReshapeFunc(reshape);
		glutReshapeWindow(windowWidth, windowHeight);
		glutIdleFunc(update);
		glutKeyboardFunc(keyboardDown);
		glutKeyboardUpFunc(keyboardUp);
		glutSpecialUpFunc(specialUp);
		glutMouseFunc(mouse);
		glutMotionFunc(mousemove);
		glutPassiveMotionFunc(passive_mousemove);
	}
	catch (const char *str)
	{
		cout << "Error During Initialiation: " << str << endl;
		for (int i = 0; i < obj_am; i++)
			delete objects[i];
		for (int i = 0; i < obj_am - 2; i++)
			delete orbits[i];
		delete[] objects;
		delete[] orbits;
		delete[] rot_periods;
		glDeleteTextures(obj_am,texId);
		delete[] texId; 
		delete[] cur_orbits_scalers;
		delete[] schem_orbits_sclaers;
		delete free_cam;
		delete sphere_cam;
		delete fly_cam;
		delete saturn_rings;
		
		//start main loop with empty screen
		glutDisplayFunc(emptydisplay);
		glutMainLoop();
		return -1;
	}


	try
	{
	glutMainLoop();
	}
	catch (const char *str)
	{
		cout << "Error During Main Loop: " << str << endl;
	}
	//release memory
	for (int i = 0; i < obj_am; i++)
		delete objects[i];
	for (int i = 0; i < obj_am - 2; i++)
		delete orbits[i];
	delete[] objects;
	delete[] orbits;
	delete[] rot_periods;
	glDeleteTextures(obj_am,texId);
	delete[] texId;
	delete[] cur_orbits_scalers;
	delete[] schem_orbits_sclaers;
	delete free_cam;
	delete sphere_cam;
	delete fly_cam;
	delete saturn_rings;
	return 0;
}
