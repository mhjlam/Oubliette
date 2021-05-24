#include "gate.h"

Gate::Gate(uint x, uint y) : Tile(x, y)
{
	Solid(true);
	Color(1.0f, 0.0f, 1.0f);

	type = TILE_GATE;
}
