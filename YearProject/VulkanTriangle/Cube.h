#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/glm.hpp>

#include <vector>
#include "Vertex.h"

class Cube
{
public:
	Cube();
	float x, y, z;
	Cube(float t_xDis, float t_yDis, float t_zDist);
	void updateColor(glm::vec3 t_color);
	void updatePos(float t_move);
	std::vector<Vertex3D> vertices = {

		// Front Face
		{ { -1.00f, -1.00f, 1.00f }, {1.0f, 0.0f, 0.0f }},
		{ { 1.00f, -1.00f , 1.00f } , { 0.0f, 1.0f, 0.0f}},
		{ {	 1.00f,  1.00f, 1.00f  } , {1.0f, 1.0f, 1.0f}},
		{ {	-1.00f,  1.00f, 1.00f } , { 0.0f, 0.0f, 1.0f}},

			// Top Face
		{ {	-1.00f,  1.00f,  1.00f } , { 1.0f, 0.0f, 0.0f}},
		{ {	 1.00f,  1.00f, 1.00f } , { 0.0f, 1.0f, 0.0f}},
		{ {	 1.00f,  1.00f, -1.00f  } , { 1.0f, 1.0f, 1.0f}},
		{ {	-1.00f,  1.00f,  -1.00f } , { 0.0f, 0.0f, 1.0f}},

	// Back Face
		{ {	 1.00f, -1.00f, -1.00f  } , { 1.0f, 0.0f, 0.0f}},
		{ {	-1.00f, -1.00f,  -1.00f } , {0.0f, 1.0f, 0.0f}},
		{ {	-1.00f,  1.00f, -1.00f } , { 1.0f, 1.0f, 1.0f}},
		{ {	 1.00f,  1.00f , -1.00f } , {0.0f, 0.0f, 1.0f}},

		// Bottom Face
		{{-1.00f, -1.00f , -1.00f } , {1.0f, 0.0f, 0.0f}},
		{{ 1.00f, -1.00f, -1.00f  } , {0.0f, 1.0f, 0.0f}},
		{{ 1.00f, -1.00f, 1.00f  } , { 01.0f, 1.0f, 1.0f}},
		{{-1.00f, -1.00f , 1.00f } , {0.0f, 0.0f, 1.0f}},

		// Left Face
		{{-1.00f, -1.00f, -1.00f } , {1.0f, 0.0f, 0.0f}},
		{{-1.00f, -1.00f , 1.00f } , {0.0f, 1.0f, 0.0f }},
		{{-1.00f,  1.00f , 1.00f } , { 1.0f, 1.0f, 0.0f}},
		{{-1.00f,  1.00f, -1.00f  } , {0.0f, 0.0f, 1.0f }},

		// Right Face
		{ { 1.00f, -1.00f, 1.00f  } , { 1.0f, 0.0f, 0.0f}},
		{ { 1.00f, -1.00f, -1.00f  } , { 0.0f, 1.0f, 0.0f}},
		{ { 1.00f,  1.00f, -1.00f  } , { 1.0f, 1.0f, 1.0f}},
		{ { 1.00f,  1.00f , 1.00f } , { 0.0f, 0.0f, 1.0f}}
	};

	const std::vector<uint32_t>indices = {
		// Front Face
		0, 1, 2,
		2, 3, 0,

		// Top Face
		4, 5, 6,
		6, 7, 4,

		// Back Face
		8, 9, 10,
		10, 11, 8,

		// Bottom Face
		12, 13, 14,
		14, 15, 12,

		// Left Face
		16, 17, 18,
		18, 19, 16,

		// Right Face
		20, 21, 22,
		22, 23, 20
	};

};

