/// \file Compiler diagnostics, basically this is either a warning or an error
/// and this represents some place in the with some message linked to it.

#pragma once

#include "location_provider.hpp"
#include "../cmd.hpp"
#include <vector>
#include <string>

struct diagnostic_type 
{
    enum _type
    {
        none = -1,
        location_warning = 0,
        location_err = 1,
        location_note = 2,
        global_warning = 8,
        global_err = 9,
        global_note = 0xA,
    } _v;

    constexpr diagnostic_type(_type v = none)
        : _v(v) {}

    constexpr bool has_location() const
    {
        return (_v & 8) == 0;
    }

    constexpr bool is_warning() const
    {
        return (_v & 7) == 0;
    }

    constexpr bool is_error() const
    {
        return (_v & 7) == 1;
    }

    constexpr bool is_note() const
    {
        return (_v & 7) == 2;
    }

    constexpr bool operator==(diagnostic_type o) const
    {
        return _v == o._v;
    }

    constexpr bool operator!=(diagnostic_type o) const
    {
        return _v != o._v;
    }
};

class diagnostic
{
public:
    diagnostic_type type;
    location_range l;
    std::string msg;

    diagnostic() = default;
    diagnostic(diagnostic const&) = delete;
    diagnostic(diagnostic &&) = default;
    diagnostic& operator=(diagnostic const&) = delete;
    diagnostic& operator=(diagnostic &&) = default;
};

class diagnostic_manager
{
protected:
    std::ostream& os;
    bool has_color;
public:
    bool error = false;

    diagnostic_manager(std::ostream& os, bool has_color)
        : os(os), has_color(has_color) {}

    // Writes the diagnostic, in a human readable way
    // Possible Improvement: Add an option to create a "machine-readable" error prompt
    void show(diagnostic const&);

    location_provider const* loc_prov = nullptr;
    std::vector<std::string> const* current_source = nullptr;
};
