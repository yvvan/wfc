#pragma once

#include <cstdint>

struct RGBA
{
	uint8_t r, g, b, a;
};

inline bool operator==(const RGBA& x, const RGBA& y)
{
	return x.r == y.r 
		&& x.g == y.g 
		&& x.b == y.b 
		&& x.a == y.a;
}
