#include "BaseObject.h"

#include <GL/glew.h>
#include <GL/glut.h>

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/matrix_inverse.hpp>

Material::Material(unsigned int prog, unsigned int tex, float amb, float diff, float spec, float gl, float a) : 
	shader_program(prog), tex_id(tex), ambient(amb), diffuse(diff), specular(spec), gloss(gl), alpha(a) {};

BaseObject::BaseObject(Material m):
	dataCount(0),
	indexesCount(0),
	pData(0),
	pIndexes(0),
	position(0, 0, 0),
	rotation(0, 0, 0),
	scale(1, 1, 1),
	obj_material(m)
{
}


BaseObject::~BaseObject(void)
{
	glDeleteBuffers(2,vbo);
	glDeleteVertexArrays(1,&vao);
	if (pData)
	{
		delete[] pData;
		delete[] pIndexes;
	}
}
void BaseObject::initGLBuffers(GLuint programId, const char* posName,const char* norName,const char* texName)
{
	glGenVertexArrays( 1, &vao );
	glBindVertexArray ( vao );

	glGenBuffers ( 2, &vbo[0]);
		
	glBindBuffer ( GL_ARRAY_BUFFER, vbo[0] );
	glBufferData ( GL_ARRAY_BUFFER, dataCount*sizeof(VertexData), pData, GL_STATIC_DRAW );
		
	glBindBuffer ( GL_ELEMENT_ARRAY_BUFFER, vbo[1] );
	glBufferData ( GL_ELEMENT_ARRAY_BUFFER, indexesCount*sizeof(unsigned int), pIndexes, GL_STATIC_DRAW );
	
	int	loc = glGetAttribLocation(programId, posName);
	if (loc>-1)
	{
		glVertexAttribPointer(loc,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)0);
		glEnableVertexAttribArray (loc);
	}
	int loc2 = glGetAttribLocation(programId, norName);
	if (loc2>-1)
	{
		glVertexAttribPointer(loc2,3,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)(0+sizeof(float)*3));
		glEnableVertexAttribArray (loc2);
	}
	int loc3 = glGetAttribLocation(programId, texName);
	if (loc3>-1)
	{
		glVertexAttribPointer(loc3,2,GL_FLOAT,GL_FALSE,sizeof(VertexData),(GLuint *)(0+sizeof(float)*6));
		glEnableVertexAttribArray (loc3);
	}
}

void BaseObject::Draw(const BaseCamera &cam, Light light) const
{

	glm::mat4x4 modelMatrix = glm::translate(glm::mat4x4(), position);
	modelMatrix = glm::rotate(modelMatrix, rotation.x, glm::vec3(1, 0, 0));
	modelMatrix = glm::rotate(modelMatrix, rotation.z, glm::vec3(0, 0, 1));
	modelMatrix = glm::rotate(modelMatrix, rotation.y, glm::vec3(0, 1, 0));
	modelMatrix = glm::scale(modelMatrix, scale);
	//modelViewMatrix consists of viewMatrix and modelMatrix
	glm::mat4x4 modelViewMatrix = cam.ViewMatrix() * modelMatrix;
	//calculate normal matrix 
	glm::mat4x4 normalMatrix = glm::inverseTranspose(modelViewMatrix);
	//finally calculate modelViewProjectionMatrix
	glm::mat4x4 modelViewProjectionMatrix = cam.ProjectionMatrix() * modelViewMatrix;

	//bind texture
	glBindTexture(GL_TEXTURE_2D, obj_material.tex_id);
  
	glm::vec4 l_pos = cam.ViewMatrix() * glm::vec4(light.position, 1);
  
	//pass variables to the shaders

	int loc = glGetUniformLocation(obj_material.shader_program, "modelViewMatrix");
	if (loc>-1)
		glUniformMatrix4fv(loc,1,0,glm::value_ptr(modelViewMatrix));
	loc = glGetUniformLocation(obj_material.shader_program, "normalMatrix");
	if (loc>-1)
	  		glUniformMatrix4fv(loc, 1, 0, glm::value_ptr(normalMatrix));
	loc = glGetUniformLocation(obj_material.shader_program, "modelViewProjectionMatrix");
	if (loc>-1)
		glUniformMatrix4fv(loc,1,0,glm::value_ptr(modelViewProjectionMatrix));
	loc = glGetUniformLocation(obj_material.shader_program, "texture");
	if (loc>-1)
		glUniform1ui(loc, 0);
	loc = glGetUniformLocation(obj_material.shader_program, "ambient");
	if (loc>-1)
		glUniform1f(loc, obj_material.ambient);
	loc = glGetUniformLocation(obj_material.shader_program, "diffuse");
	if (loc>-1)
		glUniform1f(loc, obj_material.diffuse);
	loc = glGetUniformLocation(obj_material.shader_program, "alpha");
	if (loc>-1)
		glUniform1f(loc, obj_material.alpha);
	loc = glGetUniformLocation(obj_material.shader_program, "specular");
	if (loc>-1)
		glUniform1f(loc, obj_material.specular);
	loc = glGetUniformLocation(obj_material.shader_program, "gloss");
	if (loc>-1)
		glUniform1f(loc, obj_material.gloss);
	loc = glGetUniformLocation(obj_material.shader_program, "light_source.intensity");
	if (loc>-1)
		glUniform3f(loc, light.intensity.r, light.intensity.g, light.intensity.b);
	loc = glGetUniformLocation(obj_material.shader_program, "light_source.position");
	if (loc>-1)
		glUniform3f(loc, l_pos.x, l_pos.y, l_pos.z);
	glBindVertexArray (vao);
	glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,vbo[1]);

	glDrawElements(GL_TRIANGLES,indexesCount,GL_UNSIGNED_INT,0);
	glBindBuffer(GL_ARRAY_BUFFER, NULL);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
}
