#include "wall.h"

Wall::Wall(uint x, uint y) : Tile(x, y)
{
	Solid(true);
	Color(0.5f, 0.5f, 0.5f);

	type = TILE_WALL;
}
