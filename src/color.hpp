/*
Command-Line coloring, currently works only on linux VT100-based terminals.. 
This should be overhaul, rn this is very hacky.
*/
#pragma once

#include <iostream>

enum class color
{
    red,
    blue,
    green,
    yellow,
};

std::string write_color(std::string, color);