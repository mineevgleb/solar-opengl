#pragma once
#include <glm\glm.hpp>

enum CamType{
	SPHERE,
	FREE,
	FLYING
};

class BaseCamera
{
protected:
	int * keys;
	bool * keys_down;
	int keys_am;
	glm::vec2 mouse_pos;
	glm::vec2 click_pos;
	bool is_mouse_down;
	glm::vec3 pos;
	glm::vec3 look_at;
	float field_of_view;
	float width;
	float height;
	float z_near;
	float z_far;
public:
	virtual CamType getType() = 0;
	BaseCamera(glm::vec3 pos, glm::vec3 look_at, int keys_am);
	~BaseCamera();
	virtual glm::vec3 getPosition() const;
	virtual void setPosition(glm::vec3 pos);
	virtual glm::vec3 getLookAt() const;
	virtual void setLookAt(glm::vec3 pos);
	void mouseClicked(int x, int y);
	void setNearClipPlane(float p);
	void mouseMoved(int x, int y);
	void mouseReleased();
	void setProjParams(float w, float h, double fov, double znear);
	void setActionKey(int action, int key);
	virtual void update(float dt) = 0;
	glm::mat4x4 ProjectionMatrix() const;
	virtual glm::mat4x4 ViewMatrix() const;
	void keyDown(int key);
	void keyUp(int key);
};