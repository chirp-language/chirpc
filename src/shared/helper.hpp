/*
"helper"(couldn't find a better name really), basically this is either a warning or an error
and this represents some place in the with some message linked to it
*/
#pragma once

#include <vector>
#include <string>
#include "location.hpp"

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
    // Takes in a location & file content and create a code snippet
    std::string write_helper(std::vector<std::string>);
};