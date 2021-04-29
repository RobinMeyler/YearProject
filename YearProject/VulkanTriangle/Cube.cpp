#include "Cube.h"

Cube::Cube()
{
}

Cube::Cube(float t_xDis, float t_yDis, float t_zDist)
{
	for (auto& pos : vertices)
	{
		pos.pos.x += t_xDis;
		pos.pos.y += t_yDis;
		pos.pos.z += t_zDist;
	}
}

void Cube::updateColor(glm::vec3 t_color)
{
	for (int i = 0; i < vertices.size(); i++)
	{
		vertices.at(i).color = t_color;
	}
}

void Cube::updatePos(float t_moveX, float t_moveY)
{
	for (auto& pos : vertices)
	{
		pos.pos.x += t_moveX;
		pos.pos.y += t_moveY;
	}
}
