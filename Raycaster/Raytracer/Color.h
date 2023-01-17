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
	

	Color operator*(float a)
	{
		Color color(*this);
		color.arr[0] *= a;
		color.arr[1] *= a;
		color.arr[2] *= a;

		return std::move(color);
	}

	Color operator*(Color a)
	{
		Color color(*this);
		color.arr[0] *= a.arr[0];
		color.arr[1] *= a.arr[1];
		color.arr[2] *= a.arr[2];

		return std::move(color);
	}

	Color operator+(Color a)
	{
		Color color(*this);
		color.arr[0] += a.arr[0];
		color.arr[1] += a.arr[1];
		color.arr[2] += a.arr[2];

		return std::move(color);
	}
};

std::ostream& operator<<(std::ostream& os, const Color& dt);