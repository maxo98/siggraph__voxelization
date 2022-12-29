#pragma once
#include <cstdint>

struct Color
{
public:
	Color(uint8_t _r = 0, uint8_t _g = 0, uint8_t _b = 0);

	uint8_t r, g, b;
};

