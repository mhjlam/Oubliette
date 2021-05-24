#pragma once

#include <algorithm>
#include "constants.h"
#include "structures.h"

inline float clamp(float v, float min = 0.0f, float max = 1.0f)
{
	return std::fmaxf(std::fminf(v, max), min);
}

inline void saturate(float& v)
{
	v = std::fmaxf(std::fminf(v, 1.0f), 0.0f);
}

template <typename... A>
inline float saturate(A... args)
{
	saturate(args...);
}
