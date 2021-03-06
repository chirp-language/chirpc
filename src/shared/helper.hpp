/*
"helper"(couldn't find a better name really), basically this is either a warning or an error
and this represents some place in the with some message linked to it
*/
#pragma once

#include "location.hpp"
#include "../cmd.hpp"
#include <vector>
#include <string>

enum class helper_type 
{
    global_warning,
    global_err,
    line_warning,
    line_err,
    location_warning,
    location_err
};

class helper
{
public:
    helper_type type;
    location l;
    std::string msg;

    // Writes the helper, in a human readable way
    // Possible Improvement: Add an option to create a "machine-readable" error prompt
    std::string write_helper(std::vector<std::string>, cmd&);
};