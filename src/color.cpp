#include "color.hpp"

std::string apply_color(std::string txt, color c) {
	#ifdef __unix__
	std::string result;
	// Doesn't care if it's on a VT100 terminal or not
	// will do coloring anyway.
	result += "\033[";
	unsigned int col = static_cast<unsigned int>(c);
	if ((c & color::bright) != color::blank)
		result += std::to_string(90 + (col & 7));
	else
		result += std::to_string(30 + (col & 7));
	if ((c & color::bold) != color::blank)
		result += ";1";
	result += 'm';
    result += txt;
	result += "\033[m";
	return result;
	#else
	return std::move(txt);
	#endif
}
