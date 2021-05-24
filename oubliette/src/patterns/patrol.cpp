#include "patrol.h"
#include "map.h"

Patrol::Patrol()
{
	steps_ = 0;
	dir_ = DIR_UP;
}

void Patrol::updateAI(uint& x, uint& y)
{
	uint nx = x;
	uint ny = y;

	Map* map = Map::Instance();

	if (steps_ < MAX_STEPS) // keep going in this direction
	{
		switch (dir_)
		{
			case DIR_UP:
				ny = y-1;
			break;
			case DIR_DOWN:
				ny = y+1;
			break;
			case DIR_LEFT:
				nx = x-1;
			break;
			case DIR_RIGHT:
				nx = x+1;
			break;

			default:
				return;
		}

		steps_++;
	}
	else // turn at this point
	{
		switch (dir_)
		{
			case DIR_UP:	dir_ = DIR_LEFT;	break;
			case DIR_DOWN:	dir_ = DIR_RIGHT;	break;
			case DIR_LEFT:	dir_ = DIR_DOWN;	break;
			case DIR_RIGHT: dir_ = DIR_UP;		break;
			default: return;
		}

		steps_ = 0;
	}

	if (map->isWalkable(nx, ny))
	{
		map->getTileAt(x, y)->solid = false;
		map->getTileAt(nx, ny)->solid = true;

		x = nx;
		y = ny;
	}
}
