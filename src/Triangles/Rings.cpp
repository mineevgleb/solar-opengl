#include "Ring.h"
#include "glm/gtc/constants.hpp"

void Ring::generateData()
{
	if (pData)
	{
		delete[] pData;
		delete[] pIndexes;
	}
	dataCount = (steps + 1) * 2; 
	unsigned int nTriangles = steps * 2;
	indexesCount = 3 * nTriangles;

	pData = new VertexData [dataCount];
	pIndexes = new unsigned int [indexesCount];
	for (int i = 0; i < steps + 1; i++) {
		double angle = double(i) * 2 * glm::pi<double>() / steps;
		pData[2 * i].pos.x = glm::cos(angle) * radius1;
		pData[2 * i].pos.y = 0;
		pData[2 * i].pos.z = glm::sin(angle) * radius1;;
		pData[2 * i].nor = glm::vec3(0, 1, 0);
		pData[2 * i].tex.y = 0;
		pData[2 * i].tex.x = double(i) / steps;
		pData[2 * i + 1].pos.x = glm::cos(angle) * radius2;
		pData[2 * i + 1].pos.y = 0;
		pData[2 * i + 1].pos.z = glm::sin(angle) * radius2;;
		pData[2 * i + 1].nor = glm::vec3(0, 1, 0);
		pData[2 * i + 1].tex.y = 1;
		pData[2 * i + 1].tex.x = double(i) / steps;
	}
	for (int i = 0; i < steps; i++) {
		pIndexes[i * 6] = 2 * i;
		pIndexes[i * 6 + 1] = 2 * i + 1;
		pIndexes[i * 6 + 2] = 2 * i + 2;
		pIndexes[i * 6 + 3] = 2 * i + 1;
		pIndexes[i * 6 + 4] = 2 * i + 2;
		pIndexes[i * 6 + 5] = 2 * i + 3;
	}
	initGLBuffers(obj_material.shader_program,"pos","nor","tex");
}