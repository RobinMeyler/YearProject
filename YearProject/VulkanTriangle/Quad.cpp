#include "Quad.h"

Quad::Quad()
{
}

Quad::Quad(float x, float y)
{
	for (auto &pos : vertices)
	{
		pos.pos.x += x;
		pos.pos.y += y;
	}
}

