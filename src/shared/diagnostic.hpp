/// \file Compiler diagnostics, basically this is either a warning or an error
/// and this represents some place in the with some message linked to it.

#pragma once

#include "location.hpp"
#include "location_provider.hpp"
#include "source_buffer.hpp"
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

// Forward declaration
class diagnostic_manager;

class [[nodiscard]] diagnostic
{
public:
    diagnostic_type type;
    location_range loc;
    std::string msg;

    diagnostic() = default;
    diagnostic(diagnostic_type type) : type(type) {}
    diagnostic(diagnostic const&) = delete;
    diagnostic(diagnostic &&) = default;
    diagnostic& operator=(diagnostic const&) = delete;
    diagnostic& operator=(diagnostic &&) = default;

    diagnostic&& of_type(diagnostic_type _type) && {
        type = _type;
        return std::move(*this);
    }

    diagnostic&& at(location_range _loc) && {
        loc = _loc;
        return std::move(*this);
    }

    diagnostic&& reason(std::string&& _msg) && {
        msg = std::move(_msg);
        return std::move(*this);
    }

    void report(diagnostic_manager& mng) &&;
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
    source_buffer const* current_source = nullptr;
};

inline void diagnostic::report(diagnostic_manager& mng) &&
{
    mng.show(*this);
}
