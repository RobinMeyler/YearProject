#pragma once

// Author: Robin Meyler
// Date: 2020 - 2021

#include <vector>
#include "Vertex.h"

class Quad
{
public:

	Quad();

	Quad(float x, float y);

	std::vector<Vertex2D> vertices = 
	{
		{{-0.1f, -0.1f}, {1.0f, 0.0f, 0.0f}},
		{{0.1f, -0.1f},  {0.0f, 1.0f, 0.0f}},
		{{0.1f, 0.1f}, {1.0f, 1.0f, 1.0f}},
		{{-0.1f, 0.1f}, {0.0f, 0.0f, 1.0f}}
	};

	std::vector<uint32_t> indices = 
	{
		0, 1, 2, 2, 3, 0
	};
};

