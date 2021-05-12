/*
Command-Line coloring, currently works only on linux VT100-based terminals.. 
This should be overhaul, rn this is very hacky.
*/
#pragma once

#include <iostream>

enum class color
{
    blank = 0,
    red = 1,
    green = 2,
    blue = 4,
    bright = 8,
    bold = 0x10,
};

constexpr color operator|(color a, color b)
{
    return static_cast<color>(static_cast<unsigned>(a) | static_cast<unsigned>(b));
}

constexpr color operator&(color a, color b)
{
    return static_cast<color>(static_cast<unsigned>(a) & static_cast<unsigned>(b));
}

std::string apply_color(std::string txt, color c);
