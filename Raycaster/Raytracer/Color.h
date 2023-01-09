#pragma once
#include <cstdint>
#include <iostream>

struct Color
{
public:
	Color(uint8_t _r = 0, uint8_t _g = 0, uint8_t _b = 0);

	union {
		uint8_t arr[3];
		//uint8_t r, g, b;
	};
	
};

std::ostream& operator<<(std::ostream& os, const Color& dt);