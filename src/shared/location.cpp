#include "location.hpp"

location::location()
{
    this->filename = "unknown";
    this->start = 0;
    this->line = 0;
    this->end = 0;
}

location::location(std::string f)
{
    this->filename = f;
}

location::location(int line, std::string file)
{
    this->filename = file;
    this->line = line;
}