#include "Cube.h"

Cube::Cube()
{
}

Cube::Cube(float t_xDis)
{
	for (auto& pos : vertices)
	{
		pos.pos.x += t_xDis;
	}
}
