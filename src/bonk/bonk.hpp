#pragma once

#include <string>
#include <map>

#include "value.hpp"

namespace bonk
{
    std::map<std::string, bonk::value> parse_file(std::string filename);
    std::string to_string(const std::string& name, const bonk::value& v);
}