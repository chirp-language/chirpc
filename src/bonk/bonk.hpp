#pragma once

#include <string>
#include <map>

#include "value.hpp"

namespace bonk
{
    bonk::value make_string(const std::string&);
    bonk::value make_int(const int&);
    bonk::value make_bool(const bool&);

    bonk::value make_list();
    void insert_list(bonk::value&, const std::string&, const bonk::value&);

    std::map<std::string, bonk::value> parse_file(std::string filename);
    std::string to_string(const std::string& name, const bonk::value& v);
}