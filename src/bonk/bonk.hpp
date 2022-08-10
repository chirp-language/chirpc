#pragma once

#include <string>
#include <map>

#include "value.hpp"

namespace bonk
{
    std::map<std::string, bonk::value> parse_file(std::string filename);
}