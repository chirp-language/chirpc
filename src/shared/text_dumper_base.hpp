#pragma once

#include <iostream>
#include "color.hpp"

enum class color;

class text_dumper_base
{
    public:
    bool has_colors;

    text_dumper_base(bool use_color)
        : has_colors(use_color)
    {}

    protected:
    void write_color(std::string const& txt, color clr)
    {
        ::print_color(txt, has_colors, std::cout, clr);
    }
    void begin_color(color clr)
    {
        if (has_colors)
            ::begin_color(std::cout, clr);
    }
    void end_color()
    {
        if (has_colors)
            ::end_color(std::cout);
    }
    void indent(int depth);
};
