#include "roam.h"
#include "map.h"

void Roam::updateAI(uint& x, uint& y)
{
	uint nx = x;
	uint ny = y;

	Map* map = Map::Instance();

	// get random new location (an adjacent floor)
	dir_t dir = dir_t(rand() % 4);

	switch (dir)
	{
		case DIR_UP:	ny = y-1; break;
		case DIR_DOWN:	ny = y+1; break;
		case DIR_LEFT:	nx = x-1; break;
		case DIR_RIGHT: nx = x+1; break;
		default: return;
	}

	if (map->isWalkable(nx, ny))
	{
		map->getTileAt(x, y)->solid = false;
		map->getTileAt(nx, ny)->solid = true;

		x = nx;
		y = ny;        
	}
}
