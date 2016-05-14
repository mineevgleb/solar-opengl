#pragma once
#include <glm/glm.hpp>
#include "BaseCamera.h"

//helper struct for Vertex
//contains position, normal and texture coordinates
struct VertexData
{
	 glm::vec3 pos;
	 glm::vec3 nor;
	 glm::vec2 tex;
};

struct Material 
{
	unsigned int shader_program;
	unsigned int tex_id;
	float ambient;
	float diffuse;
	float specular;
	float gloss;
	float alpha;
	Material (unsigned int prog, unsigned int tex, float amb = 0.2f, float diff = 0.7f, float spec = 1.2f, float gl = 2.0f, float a = 1.0f);
};

struct Light {
	glm::vec3 intensity;
	glm::vec3 position;
};

//some object for drawing
class BaseObject
{
	unsigned int vbo[2];//VertexBufferObject one for MeshVertexData, another for Indexes
	unsigned int vao;//one VertexArrayObject
public:
	glm::vec3 position;
	glm::vec3 rotation;
	glm::vec3 scale;
protected:
	VertexData* pData;	//pointer to object's internal data
	unsigned int dataCount;

	unsigned int* pIndexes;	//pointer to indexes (list of vetrices) 
	unsigned int indexesCount;
public:
	BaseObject(Material m);
	~BaseObject(void);
	Material obj_material;
	//function for initialization
	void initGLBuffers(unsigned int programId, const char* posName,const char* norName,const char* texName);
	//function for drawing
	void Draw(const BaseCamera &cam, Light light) const;
protected:
	virtual void generateData() = 0;
};

