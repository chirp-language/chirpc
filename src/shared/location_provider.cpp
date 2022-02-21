#include "location_provider.hpp"
#include "system.hpp"
#include <string>

void location_provider::begin_run(location_run& run)
{
	if (_current_run)
		chirp_unreachable("Location run already in progress");
	_current_run = &run;
	_current_run->last_loc = nullptr;
}

void print_loc_single(location const& loc, std::string& str, location_run* run)
{
	// There are 3 cases:
	// [has_miss or fname mismatch or line invalid]
	// <filename>:<line>|invalid:<col>|invalid
	// [line mismatch or col invalid]
	// :line:<line>:<col>|invalid
	// [otherwise]
	// :col:<col>
	bool has_miss = !run or !run->last_loc;

	if (has_miss or loc.filename != run->last_loc->filename or loc.line == -1)
	{
		has_miss = true;
		str += loc.filename;
		str += ":";
	}

	if (loc.line == -1)
	{
		has_miss = true;
		str += "invalid:";
	}
	else if (has_miss or loc.line != run->last_loc->line or loc.start == -1)
	{
		if (!has_miss)
		{
			str += ":line:";
			has_miss = true;
		}
		str += std::to_string(loc.line+1);
		str += ":";
	}

	if (loc.start == -1)
	{
		str += "invalid";
	}
	else
	{
		if (!has_miss)
		{
			str += ":col:";
		}
		str += std::to_string(loc.start+1);
	}

	if (run)
		run->last_loc = &loc;
}

std::string location_provider::print_loc(location_range loc) const
{
	auto const& locb = get_loc(loc.begin);
	auto const& loce = get_loc(loc.end);
	std::string result;
	result += "<";
	print_loc_single(locb, result, _current_run);
	if (&locb != &loce)
	{
		result += ", ";
		print_loc_single(loce, result, _current_run);
	}
	result += ">";
	return result;
}
