#include "monster.h"

monster::monster(uint x, uint y) : entity(x, y)
{
	set_color(1.0f, 0.0f, 0.0f);
}

void monster::update()
{
	if (!alive) return;
	ai_pattern->updateAI(x, y);
}
