#include "color.hpp"

void begin_color(std::ostream& os, color clr)
{
	#ifdef __unix__
	// Doesn't care if it's on a VT100 terminal or not
	// will do coloring anyway.
	os << "\033[";
	unsigned int col = static_cast<unsigned int>(clr);
	if ((clr & color::bright) != color::blank)
		os << (90 + (col & 7));
	else
		os << (30 + (col & 7));
	if ((clr & color::bold) != color::blank)
		os << ";1";
	os << 'm';
	#endif
}

void end_color(std::ostream& os)
{
	#ifdef __unix__
	os << "\033[m";
	#endif
}
