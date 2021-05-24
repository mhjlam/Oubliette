#pragma once

// enums
enum tile_t
{
	TILE_WALL,
	TILE_FLOOR,
	TILE_DOOR,
	TILE_GOLD,
	TILE_KEY,
	TILE_ROOM,
	TILE_GATE
};

enum dir_t
{
	DIR_UP,
	DIR_DOWN,
	DIR_LEFT,
	DIR_RIGHT,
	DIR_NONE
};

// structs
struct color_t
{
	float r, g, b;

	color_t() {}
	color_t(float r, float g, float b) : r(r), g(g), b(b) {}
};

struct bullet_t
{
	uint x, y;
	dir_t direction;
	color_t color;

	bullet_t() {};
	bullet_t(uint x, uint y, dir_t d, color_t c) : x(x), y(y), direction(d), color(c) {}
};
