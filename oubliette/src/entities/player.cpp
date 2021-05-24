#include "player.h"
#include <iostream>

player::player(const std::string& name, uint x, uint y) : 
	entity(x, y), name(name), gold(0), keys(0), gate_key(false), in_room(false), bullet(nullptr)
{
	color = color_t(0.0f, 1.0f, 0.0f);
}

player::~player()
{
	destroy_bullet();
}

void player::shoot()
{
	if (bullet == nullptr) // only one allowed at a time
	{
		bullet = new bullet_t(x, y, direction, color);
	}
}

void player::destroy_bullet()
{
	delete bullet;
	bullet = nullptr;
}

void player::kill()
{
	entity::kill();
	if (gate_key) std::cout << name << " still has the gate key!" << std::endl;
}
