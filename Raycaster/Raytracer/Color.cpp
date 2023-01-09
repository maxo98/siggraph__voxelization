#include "Color.h"

Color::Color(uint8_t _r, uint8_t _g, uint8_t _b)
{
	arr[0] = _r;
	arr[1] = _g;
	arr[2] = _b;
}

std::ostream& operator<<(std::ostream& os, const Color& dt)
{
	//os << (int)dt.r << " " << (int)dt.g << " " << (int)dt.b;

	return os;
}
