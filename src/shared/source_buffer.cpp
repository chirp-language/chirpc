#include "source_buffer.hpp"

#include <algorithm>
#include <cstring>

source_buffer::source_buffer(std::string && _source)
    : source(std::move(_source))
{
    // Find lines
    line_map.reserve(8);
    line_map.push_back(0);
    size_t index = 0;
    while (index < source.size())
    {
        char ch = source[index];
        bool new_line = false;
        // This handles both "\n", "\r", and "\r\n"
        if (ch == '\r')
        {
            ++index;
            new_line = true;
            if (index < source.size())
                ch = source[index];
        }
        if (ch == '\n')
        {
            ++index;
            new_line = true;
        }
        if (new_line and index < source.size())
            line_map.push_back(index);
        else
            ++index;
    }
}

size_t source_buffer::next_line_after(size_t pos) const
{
    auto it = std::lower_bound(line_map.begin(), line_map.end(), pos);
    if (it == line_map.end())
        return npos;
    return *it;
}

std::string_view source_buffer::at(size_t line) const
{
    chirp_assert(line < line_map.size(), "Line is out of range");
    size_t beg = line_map[line];
    size_t end = line + 1 == line_map.size() ? npos : line_map[line+1];
    auto range = std::string_view(source).substr(beg, end - beg);
    while (range.size() and std::strchr("\n\r", range.back()))
        range.remove_suffix(1);
    return range;
}
