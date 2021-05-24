#include "key.h"

Key::Key(uint x, uint y) : Tile(x, y)
{
	Solid(false);
	Color(0.0f, 1.0f, 1.0f);

	type = TILE_KEY;
}
