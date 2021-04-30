#pragma once

#include "../lexer/token.hpp"

class location_provider
{
	public:
	virtual location const& get_loc(token_location) const = 0;

	std::string print_loc(location_range) const;
};
