#pragma once

#include <algorithm>
#include "../constants.h"
#include "../structures.h"
#include "../functions.h"

// Movable game object
class entity
{
public:
	uint x, y;
	bool alive;
	color_t color;
	dir_t direction;

public:
	entity() : x(0), y(0), alive(true), direction(DIR_UP) {}
	entity(uint x, uint y) : x(x), y(y), alive(true), direction(DIR_UP) { set_color(0, 0, 0); }
	virtual ~entity() { kill(); }

public:
	virtual void update() {}
	virtual void kill() { alive = false; };

	virtual void set_color(float r, float g, float b)
	{
		// clamp values to 0 < n < 1
		//saturate(r, g, b);
		color.r = clamp(r);
		color.g = clamp(g);
		color.b = clamp(b);
	}
};
