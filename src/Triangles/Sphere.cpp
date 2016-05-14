#include "Sphere.h"
#include <GL/glew.h>
#include <GL/glut.h>
#include <glm/gtc/constants.hpp>
#include <map>
#include <vector>
#include <utility>
#include <algorithm>
#include <iostream>
#include <cmath>

void Sphere::tesselate(VertexData *&vert, unsigned int *&ind, unsigned int &vert_am, unsigned int &ind_am)
{
	std::map<std::pair<unsigned int, unsigned int>, unsigned int> edge_centers;
	std::pair<int, int> a;
	std::vector<VertexData> new_vert(vert, vert + vert_am);
	std::vector<unsigned int> new_ind;
	for (unsigned int i = 0; i < ind_am / 3; i++) {
		unsigned int cur_vert_num[6];
		for (unsigned int j = 0; j < 3; j++) {
			cur_vert_num[j] = ind[i * 3 + j];
		}
		for (unsigned int j = 0; j < 3; j++) {
			std::pair<unsigned int, unsigned int> edge;
			edge.first = std::min(ind[i * 3 + j], ind[i * 3 + (j + 1) % 3]);  
			edge.second = std::max(ind[i * 3 + j], ind[i * 3 + (j + 1) % 3]);
			auto it = edge_centers.find(edge);
			if (it != edge_centers.end()) {
				cur_vert_num[3 + j] = it->second;
			} else {
				VertexData v;
				v.pos = vert[edge.first].pos + vert[edge.second].pos;
				v.pos /= 2;
				new_vert.push_back(v);
				std::pair<std::pair<unsigned int, unsigned int>, unsigned int> new_elem;
				new_elem.first = edge;
				new_elem.second = new_vert.size() - 1;
				edge_centers.insert(new_elem);
				cur_vert_num[3 + j] = new_elem.second;
			}
		}
		new_ind.push_back(cur_vert_num[0]); new_ind.push_back(cur_vert_num[5]); new_ind.push_back(cur_vert_num[3]);
		new_ind.push_back(cur_vert_num[5]); new_ind.push_back(cur_vert_num[2]); new_ind.push_back(cur_vert_num[4]);
		new_ind.push_back(cur_vert_num[3]); new_ind.push_back(cur_vert_num[4]); new_ind.push_back(cur_vert_num[1]);
		new_ind.push_back(cur_vert_num[3]); new_ind.push_back(cur_vert_num[4]); new_ind.push_back(cur_vert_num[5]);
	}
	ind_am = new_ind.size();
	vert_am = new_vert.size();
	delete[] ind;
	delete[] vert;
	ind = new unsigned int[ind_am];
	vert = new VertexData[vert_am];
	ind[0] = 0;
	memcpy(ind, &new_ind[0], sizeof(*ind) * ind_am);
	memcpy(vert, &new_vert[0], sizeof(*vert) * vert_am);
	for (unsigned int i = 0; i < vert_am; i++) {
		vert[i].pos = glm::normalize(vert[i].pos);
	}
}

void Sphere::make_seam(VertexData *&vert, unsigned int *&ind, unsigned int &vert_am, unsigned int &ind_am)
{
	std::vector<VertexData> new_vert(vert, vert + vert_am);
	std::map<unsigned int, unsigned int> vertex_map; 
	for (unsigned int i = 0; i < ind_am / 3; i++) {
		for (int j = 0; j < 3; j++) {
			unsigned int pt1 = ind[i * 3 + j];
			unsigned int pt2 = ind[i * 3 + (j + 1) % 3];
			if (pt1 > vert_am || pt2 > vert_am) continue;
			if (vert[pt1].tex.x > vert[pt2].tex.x) {
				unsigned int t;
				t = pt1;
				pt1 = pt2;
				pt2 = t;
			}
			if (vert[pt2].tex.x - vert[pt1].tex.x > 0.5 && pt1 < vert_am && pt2 < vert_am) {
				auto it = vertex_map.find(pt1);
				if (it == vertex_map.end()) {
					VertexData v = vert[pt1];
					v.tex.x += 1;
					new_vert.push_back(v);
					vertex_map.insert(std::pair<unsigned int, unsigned int>(pt1, new_vert.size() - 1));
					if (ind[i * 3 + j] == pt1) 
						ind[i * 3 + j] = new_vert.size() - 1;
					if (ind[i * 3 + (j + 1) % 3] == pt1) 
						ind[i * 3 + (j + 1) % 3] = new_vert.size() - 1;
				} else {
					if (ind[i * 3 + j] == pt1) 
						ind[i * 3 + j] = it->second;
					if (ind[i * 3 + (j + 1) % 3] == pt1) 
						ind[i * 3 + (j + 1) % 3] = it->second;
				}
			}
		}
	}
	vert_am = new_vert.size();
	delete[] vert;
	vert = new VertexData[vert_am];
	memcpy(vert, &new_vert[0], sizeof(*vert) * vert_am);
	
}

void Sphere::generateData()
{
	if (pData)
	{
		delete[] pData;
		delete[] pIndexes;
	}
	dataCount = 12; 
	unsigned int nTriangles = 20;
	indexesCount = 3*nTriangles;

	pData = new VertexData [dataCount];
	pIndexes = new unsigned int [indexesCount];
	pData[10].pos = glm::vec3(0, 1, 0);
	pData[11].pos = glm::vec3(0, -1, 0);
	float angle_v = glm::atan(1.0f / 2);
	float angle_h = 0;
	for (int i = 0; i < 10; i++) {
		pData[i].pos.x = glm::sin(angle_v + glm::pi<float>() / 2) * glm::cos(angle_h);
		pData[i].pos.y = glm::cos(angle_v + glm::pi<float>() / 2);
		pData[i].pos.z = glm::sin(angle_v + glm::pi<float>() / 2) * glm::sin(angle_h);
		angle_h += glm::pi<float>() / 5;
		angle_v *= -1;
	}
	for (int i = 0; i < 10; i+=2) {
		pIndexes[i * 3] = 10;
		pIndexes[(i + 1) * 3] = 11;
		pIndexes[i * 3 + 1] = ((i * 2) + 1) % 10;
		pIndexes[i * 3 + 2] = ((i * 2) + 3) % 10;
		pIndexes[(i + 1) * 3 + 1] = ((i * 2)) % 10;
		pIndexes[(i + 1) * 3 + 2] = ((i * 2) + 2) % 10;
	}
	for (int i = 0; i < 10; i++) {
		pIndexes[(i + 10) * 3] = (i) % 10;
		pIndexes[(i + 10) * 3 + 1] = (i + 1) % 10;
		pIndexes[(i + 10) * 3 + 2] = (i + 2) % 10;
	}
	for (unsigned int i = 0; i < iter_am; i++)
		tesselate(pData, pIndexes, dataCount, indexesCount);
	for (unsigned int i = 0; i < dataCount; i++) {
		pData[i].nor = pData[i].pos;
		pData[i].tex.x = atan2f(pData[i].pos.z, pData[i].pos.x) / (2 * glm::pi<float>()) + 0.5f;
		pData[i].tex.y = -glm::asin(pData[i].pos.y) / (glm::pi<float>()) + 0.5f;
	}
	make_seam(pData, pIndexes, dataCount, indexesCount);
	for (unsigned int i = 0; i < dataCount; i++) {
		pData[i].pos *= radius;	
	}
	initGLBuffers(obj_material.shader_program,"pos","nor","tex");
}