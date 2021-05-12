#include "location_provider.hpp"
#include "../color.hpp"

std::string location_provider::print_loc(location_range loc) const
{
	auto locb = get_loc(loc.begin);
	location const* loce = loc.end == loc.begin ? nullptr : &get_loc(loc.end);
	std::string result;
	result += "<";
	result += locb.filename;
	result += ":";
	if (locb.line == -1)
		result += "invalid";
	else
		result += std::to_string(locb.line+1);
	result += ":";
	if (locb.start == -1)
		result += "invalid";
	else
		result += std::to_string(locb.start+1);
	if (loce)
	{
		result += ", ";
		result += loce->filename;
		result += ":";
		if (loce->line == -1)
			result += "invalid";
		else
			result += std::to_string(loce->line+1);
		result += ":";
		if (loce->start == -1)
			result += "invalid";
		else
			result += std::to_string(loce->start+1);
	}
	result += ">";
	return result;
}
