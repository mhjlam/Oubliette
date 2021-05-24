#include "gold.h"

Gold::Gold(uint worth, uint x, uint y) : Tile(x, y), value_(worth)
{
	Solid(false);
	Color(1.0f, 1.0f, 0.0f);

	type = TILE_GOLD;
}
