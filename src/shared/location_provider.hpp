#pragma once

#include "../lexer/token.hpp"
#include "location.hpp"

class location_provider
{
	location_run* _current_run = nullptr;

	public:
	virtual location const& get_loc(token_location) const = 0;

	std::string print_loc(location_range) const;
	void begin_run(location_run& run);
	void end_run()
	{
		_current_run = nullptr;
	}
};
