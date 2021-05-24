#include "map.h"
#include <fstream>
#include <iostream>
#include "tiles/room.h"
#include "tiles/floor.h"
#include "patterns/roam.h"
#include "patterns/patrol.h"
#include "entities/player.h"

Map* Map::instance_ = nullptr;

Map* Map::Instance()
{
	if (instance_ == nullptr)
		instance_ = new Map();

	return instance_;
}

void Map::clearMap()
{
	for (uint i = 0; i < tiles_.size(); i++)
	{
		delete tiles_.at(i);
		tiles_.at(i) = nullptr;
	}

	tiles_.clear();

	roomWithGateKey_ = nullptr;
}

void Map::createMap(const std::string& filename, std::vector<entity*>& entities) // create all entities, set their locations, colors, etc
{
	std::ifstream file(filename.c_str(), std::ios::in);

	if (!file.good() || !file.is_open())
	{
		std::cout << "Error: " << filename << " is not a valid map or does not exist" << std::endl;
		return;
	}

	std::string line;

	// reset map properties
	clearMap();
	mapMaxWidth_ = 0;

	// reserve first two entries for players
	entities.clear();
	entities.push_back(NULL);
	entities.push_back(NULL);

	uint y = 0;

	while (file.good() && !file.eof())
	{
		line.clear();

		try
		{
			std::getline(file, line);
		}
		catch (std::ios_base::failure& e)
		{
			std::cout << "Warning: An error occurred while reading map file, " << e.what();
			break; // break from the while loop, file will be closed
		}

		if (!line.empty())
		{
			if (mapMaxWidth_ < line.size())
				mapMaxWidth_ = (uint)line.size();

			for (uint x = 0; x < line.size(); x++)
				parseChar(line.at(x), x, y, entities);

			++y;
		}

		mapMaxHeight_ = y;
	}

	file.close();

	// Fail-safe entity creation:

	// no player was created
	if (!entities.at(0))
	{
		std::cout << "Warning: Map does not contain any players, will randomly generate one" << std::endl;

		Tile* location = findRandomFloorTile();

		player* p = new player("Player1", location->x, location->y);
		location->solid = true;
		entities.at(0) = p;
	}

	if (!roomWithGateKey_) // no room with gate key
	{
		// if there are no rooms at all, we will have to create one manually
		bool generateNewRoom = true;

		for (uint i = 0; i < tiles_.size(); i++)
		{
			if (tiles_.at(i)->type == TILE_ROOM)
			{
				roomWithGateKey_ = (Room*)tiles_.at(i);
				generateNewRoom = false;
				break; // stop searching
			}
		}

		// the map contained no room at all
		if (generateNewRoom)
		{
			Tile* location = findRandomFloorTile();

			Room* r = new Room(location->x, location->y);
			roomWithGateKey_ = r;

			replaceTile(r, location->x, location->y);
		}
	}

	if (!gate_) // no gate
	{
		std::cout << "Warning: Map does not contain an exit gate, will randomly generate one" << std::endl;

		Tile* location = findRandomFloorTile();

		gate_ = new Gate(location->x, location->y);
		replaceTile(gate_, location->x, location->y);
	}

	// set the room with the gatekey (which should exist at this point)
	Room::setRoomWithGateKey(roomWithGateKey_);

	// print some warning messages if something is wrong with map dimensions
	if (getMapWidth() == 0)
		std::cout << "Warning: Map width is 0, nothing to display!" << std::endl;
	else if (getMapHeight() == 0)
		std::cout << "Warning: Map height is 0, nothing to display!" << std::endl;
	else
	{
		if (getMapWidth() < 3 || getMapHeight() < 3)
			std::cout << "Warning: The map appears to be a little small, something may have gone wrong" << std::endl;

		if (getMapWidth() != getMapHeight())
			std::cout << "Warning: Map width and height are not equal, the map may appear stretched" << std::endl;
	}
}

void Map::parseChar(const char c, uint x, uint y, std::vector<entity*>& entities)
{
	Tile* tile = NULL;

	switch (c)
	{
		case(MAP_CHAR_PLAYER1):
		{
			if (!entities.at(0))
			{
				player* pl1 = new player("Player1", x, y);
				pl1->set_color(0.0f, 1.0f, 0.0f); // green
				entities.at(0) = pl1; // was reserved for player1
			}

			tile = new Floor(x, y);
			tile->solid = true; // player occupies this tile
		}
		break;

		case(MAP_CHAR_PLAYER2):
		{
			if (!entities.at(1))
			{
				player* pl2 = new player("Player2", x, y);
				pl2->set_color(0.0f, 0.0f, 1.0f); // blue
				entities.at(1) = pl2; // was reserved for player2
			}

			tile = new Floor(x, y);
			tile->solid = true; // player occupies this tile
		}
		break;

		case(MAP_CHAR_MONSTER):
		{
			monster* m = new monster(x, y);
			// give Monster either a roamer or patrol AI (roamer is more likely)
			(rand()%3 == 1) ? m->set_pattern(new Patrol()) : m->set_pattern(new Roam());

			entities.push_back(m); // monsters are the only entities other than the players

			tile = new Floor(x, y);
			tile->solid = true; // monster occupies this tile
		}
		break;

		case(MAP_CHAR_WALL):
			tile = new Wall(x, y);
		break;

		case(MAP_CHAR_ROOM):
		{
			tile = new Room(x, y);

			// random chance of making this the room that will contain the gate key
			if (roomWithGateKey_ == NULL)
				if (rand() % 3 == 1)
					roomWithGateKey_ = tile;
		}
		break;

		case(MAP_CHAR_DOOR):
			tile = new Door(x, y);
		break;

		case(MAP_CHAR_GOLD):
			tile = new Gold(rand() % 125 + 20, x, y);
		break;

		case(MAP_CHAR_KEY):
			tile = new Key(x, y);
		break;

		case(MAP_CHAR_EXIT):
			tile = new Gate(x, y);
			gate_ = (Gate*)tile;
		break;

		default:
			tile = new Floor(x, y);
		break;
	}

	tiles_.push_back(tile);
}

Tile* const Map::findRandomFloorTile()
{
	Tile* freeSpace = NULL;

	uint x = 0;
	uint y = 0;

	// to prevent this from looping forever, at least empty space should be present (what kind of map would it be if it didn't)
	// still, it's hardly efficient... but it's the only true random method we can think of
	do
	{
		x = rand() % (mapMaxWidth_ - 5) + 5;
		y = rand() % (mapMaxHeight_ - 5) + 5;

		freeSpace = getTileAt(x, y);
	}
	while (!freeSpace || !isWalkable(x, y) || !isType(TILE_FLOOR, x, y));

	return freeSpace;
}

void Map::replaceTile(Tile* newTile, uint x, uint y)
{
	// we're not placing NULL's
	if (!newTile)
		return;

	// don't replace if the coordinates are incorrect
	if (newTile->x != x || newTile->y != y)
		return;

	Tile* replacement = getTileAt(x, y);

	if (!replacement) // no tile was ever here
		tiles_.push_back(newTile);
	else
	{
		// locate the tile in the vector
		std::vector<Tile*>::iterator it;

		for (it = tiles_.begin(); it != tiles_.end(); ++it)
		{
			if (*it == replacement)
			{
				delete *it; // make sure the placeholder is destroyed
				*it = newTile;

				break;
			}
		}

	}
}

void Map::putFloorAt(uint x, uint y)
{
	Tile* location = getTileAt(x, y);

	// location doesn't exist
	if (!location)
		return;

	tile_t type = location->type;

	// only these types of tile are allowed to be replaced
	if (type == TILE_DOOR || type == TILE_GATE || type == TILE_GOLD || type == TILE_KEY)
		replaceTile(new Floor(x, y), x, y);
}

void Map::putGoldAt(uint x, uint y, uint amount)
{
	Tile* location = getTileAt(x, y);

	// location doesn't exist
	if (!location)
		return;

	if (location->type == TILE_FLOOR)
		replaceTile(new Gold(rand() % 125 + 20, x, y), x, y);
}

bool Map::isWalkable(uint x, uint y) const
{
	if (x < 0 || x > mapMaxWidth_ || y < 0 || y > mapMaxHeight_) // out of map boundaries
		return false;

	if (getTileAt(x, y)->solid == true)
		return false;

	return true;
}

bool Map::isType(tile_t type, uint x, uint y) const
{
	Tile* t = getTileAt(x, y);

	if (t && t->type == type)
		return true;

	return false;
}

Tile* const Map::getTileAt(uint x, uint y) const
{
	Tile* tile = NULL;

	for (uint i = 0; i < tiles_.size(); i++)
	{
		if (tiles_.at(i)->x == x && tiles_.at(i)->y == y)
		{
			tile = tiles_.at(i);
			break;
		}
	}

	return tile;
}
