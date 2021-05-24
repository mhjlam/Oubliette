/*
 * 	Tiles make up the grid of the map. All elements on the map are Tiles, except for moving objects, which are Entities.
 *  An important property is the tile type, which is used throughout the program to be able to
 *  get information about the environment. Tiles can be of the types as defined in Constants.h
 *  Tiles can be solid/non-solid (if solid, then Entity cannot move there).
 *  Individual Tiles generally just set their respective colors and type, and these values are requested by Game when required.
 */

#pragma once

#include <cmath>
#include "../constants.h"
#include "../functions.h"
#include "../structures.h"

class Tile
{
public:
	uint x, y;
	bool solid;
	tile_t type;
	color_t color;

public:
	Tile() : x(0), y(0), solid(true) {} // required by std::vector
	Tile(uint x, uint y, tile_t type) : x(x), y(y), solid(true), type(type) { color = color_t(0.0f, 0.0f, 0.0f); }
};

class Door : public Tile
{
public:
	Door(uint x, uint y) : Tile(x, y, TILE_DOOR)
	{
		solid = true;
		color = color_t(1.0f, 1.0f, 1.0f);
	}
};

class Floor : public Tile
{
public:
	Floor(uint x, uint y) : Tile(x, y, TILE_FLOOR)
	{
		solid = false;
		color = color_t(0.0f, 0.0f, 0.0f);
	}
};

class Gate : public Tile
{
public:
	Gate(uint x, uint y) : Tile(x, y, TILE_GATE)
	{
		solid = true;
		color = color_t(1.0f, 0.0f, 1.0f);
	}
};

class Gold : public Tile
{
public:
	uint value_;
	Gold(uint worth, uint x, uint y) : Tile(x, y, TILE_GOLD), value_(worth)
	{
		solid = false;
		color = color_t(1.0f, 1.0f, 0.0f);
	}
};

class Key : public Tile
{
public:
	Key(uint x, uint y) : Tile(x, y, TILE_KEY)
	{
		solid = false;
		color = color_t(0.0f, 1.0f, 1.0f);
	}
};

class Wall : public Tile
{
public:
	Wall(uint x, uint y) : Tile(x, y, TILE_WALL)
	{
		solid = true;
		color = color_t(0.5f, 0.5f, 0.5f);
	}
};
