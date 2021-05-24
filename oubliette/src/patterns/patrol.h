#pragma once

#include "pattern.h"
#include "../structures.h"

class Patrol : public pattern
{
private:
	static const uint MAX_STEPS = 2;		// max number of steps before turning

	uint steps_;							// current steps taken
	dir_t dir_;							// direction Entity is facing

public:
	Patrol();
	void updateAI(uint& x, uint& y);
};
