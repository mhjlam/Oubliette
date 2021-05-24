#pragma once

#include "pattern.h"

// Move in random direction
class Roam : public pattern
{
public:
	void updateAI(uint& x, uint& y);
};
