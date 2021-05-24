#include "floor.h"

Floor::Floor(unsigned x, unsigned y) : Tile(x, y)
{
	Solid(false);
	Color(0.0f, 0.0f, 0.0f);

	type = TILE_FLOOR;
}
