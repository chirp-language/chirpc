#pragma once

#include <string>
#include <map>

#include "value.hpp"

namespace bonk
{
    bonk::list parse_file(std::string filename);
    std::string serialize(const std::string& name, const bonk::value& v);
}
