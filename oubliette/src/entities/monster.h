#pragma once

#include "../entities/entity.h"
#include "../patterns/pattern.h"

class monster : public entity
{
private:
	pattern* ai_pattern;

public:
	monster(uint x, uint y);

	void update();
	void set_pattern(pattern* ai) { ai_pattern = ai; }
};
