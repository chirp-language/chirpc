#pragma once

#include <string>

enum class color;

class text_dumper_base
{
    public:
    bool has_colors;

    text_dumper_base(bool enable_colors)
        : has_colors(enable_colors)
    {}

    protected:
    void write_color(std::string, color);
    void begin_color(color);
    void end_color();
    void indent(int depth);
};
