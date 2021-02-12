/*
"helper"(couldn't find a better name really), basically this is either a warning or an error
and this represents some place in the with some message linked to it
*/
#pragma once
#include <string>
#include "location.hpp"
enum helper_type 
{
    global_warning,
    global_err,
    location_warning,
    location_err
};

class helper{
    public:
    helper_type type;
    location l;
    std::string msg;
    std::string file; // Filename for the file, very hacky..
};