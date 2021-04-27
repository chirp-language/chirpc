#include "location.hpp"

location::location()
    : filename("<unknown>")
{}

location::location(std::string f)
    : filename(std::move(f))
{}

location::location(int line, std::string file)
    : filename(std::move(file)), line(line)
{}
