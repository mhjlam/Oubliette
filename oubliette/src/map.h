/*
 * 	1) Reads and parses a given map file (text) based on that characters defined in Constants.h
 *  2) Manages the dimensions of the map and all the Tiles that exist on the map.
 *     As such, it has functions that other classes may use to get information about the environment,
 *     or to alter the map in some way.
 */

#pragma once

#include <string>
#include <vector>

#include "constants.h"
#include "tiles/tile.h"
#include "tiles/gate.h"
#include "entities/entity.h"

class Map
{
private:
	static Map* instance_;															// singleton

	std::vector<Tile*> tiles_;
	Tile* roomWithGateKey_; 														// there is one single room that contains a gate key
	Gate* gate_;																	// a level/map must have at least one gate

	uint mapMaxWidth_, mapMaxHeight_;												// map dimensions

	// functions
	void clearMap(); 																// clears map (so another can be read)
	void parseChar(const char c, uint x, uint y, std::vector<entity*>& entities);	// parses a character from Constants.h and creates a corresponding Tile or Entity

private:
	Map() : roomWithGateKey_(nullptr), gate_(nullptr), mapMaxWidth_(0), mapMaxHeight_(0) {}
	~Map() { clearMap(); delete instance_; instance_ = nullptr; }

public:
	static Map* Instance();	// singleton

public:
	void createMap(const std::string& filename, std::vector<entity*>& entities);	// read a text file and set the general data of the map
																					// (includes ensuring there are several required elements on the map)
	Tile* const findRandomFloorTile();												// random "empty" Floor Tile that can be used to place something on
	void replaceTile(Tile* newTile, uint x, uint y);								// replace the Tile at (x,y) with another (type of) Tile
	void putFloorAt(uint x, uint y);												// same as replaceTile, but specifically to replace Tile at (x,y) with a Floor Tile
	void putGoldAt(uint x, uint y, uint amount);									// same as replaceTile, but specifically to replace Tile at (x,y) with a Goldbag Tile

	bool isWalkable(uint x, uint y) const;											// returns whether this tile is walkable (in boundaries and non-solid)

	uint getMapWidth() const { return mapMaxWidth_; };								// returns map width, based on map dimensions
	uint getMapHeight() const { return mapMaxHeight_; };							// returns map height, based on map dimensions

	bool isType(tile_t type, uint x, uint y) const;								// returns whether the Tile at (x,y) is of the given type
	Tile* const getTileAt(uint x, uint y) const;									// returns a pointer to the Tile at (x,y)
	const std::vector<Tile*>& getTiles() const { return tiles_; };					// returns a vector of all the Tiles created during parsing of the map
};
