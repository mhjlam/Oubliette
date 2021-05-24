#include "door.h"

Door::Door(uint x, uint y) : Tile(x, y)
{
	Solid(true);
	Color(1.0f, 1.0f, 1.0f);

	type = TILE_DOOR;
}
