#pragma once

#include <string>
#include "../entities/entity.h"

class player : public entity
{
public:
	std::string name;
	uint gold;
	uint keys;
	bool gate_key;
	bool in_room;
	bullet_t* bullet;

public:
	player(const std::string& name, uint x, uint y);
	~player();

	void kill();
	void shoot(); // using x, y, direction, player color
	void destroy_bullet(); // when it hits a wall, monster, or other player
};
