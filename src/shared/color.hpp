/*
Command-Line coloring, currently works only on linux VT100-based terminals.. 
This should be overhaul, rn this is very hacky.
*/
#pragma once

#include <ostream>

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

void begin_color(std::ostream& os, color clr);
void end_color(std::ostream& os);

struct color_scope
{
    bool use_color;
    std::ostream& os;
    color_scope(bool use_color, std::ostream& os, color clr)
        : use_color(use_color), os(os)
    {
        if (use_color)
            begin_color(os, clr);
    }
    ~color_scope()
    {
        if (use_color)
            end_color(os);
    }
};

inline void print_color(std::string const& txt, bool use_color, std::ostream& os, color clr)
{
    color_scope cs(use_color, os, clr);
    os << txt;
}
