#pragma once

#include <string>
#include <vector>

#include "system.hpp"

class source_buffer
{
    std::string source;
    std::vector<size_t> line_map;

public:
    source_buffer(std::string && source);

    std::string_view get() const { return source; }
    size_t line_count() const { return line_map.size(); }

    constexpr static size_t npos = ~static_cast<size_t>(0);

    // Find next line that begins on or after pos.
    // Returns the position of the beginning of the line
    // or ~static_cast<size_t>(0) = npos if not found.
    // Lines are found to be terminated by one of ("\n", "\r", "\r\n").
    size_t next_line_after(size_t pos) const;

    // Lines are numbered 0..#lines-1
    size_t get_line_position(size_t line) const
    {
        chirp_assert(line < line_map.size(), "Line is out of range");
        return line_map[line];
    }

    std::string_view at(size_t line) const;

    struct const_iterator
    {
        source_buffer const& ref;
        size_t pos, next;

        const_iterator(source_buffer const& ref, size_t p)
            : ref(ref)
            , pos(p)
        {
            next = pos == npos ? npos : ref.next_line_after(pos + 1);
        }

        std::string_view operator*() const
        {
            return ref.get().substr(pos, next - pos);
        }

        const_iterator& operator++()
        {
            pos = next;
            if (pos != npos)
                next = ref.next_line_after(pos + 1);
            return *this;
        }

        bool operator==(const_iterator const& other) const
        {
            return &ref == &other.ref and pos == other.pos;
        }
        #ifndef CHIRP_CXX_20
        bool operator!=(const_iterator const& other) const { return !(*this == other); }
        #endif
    };
    using iterator = const_iterator;

    const_iterator begin() const { return { *this, 0 }; }
    const_iterator end() const { return { *this, npos }; }
};
