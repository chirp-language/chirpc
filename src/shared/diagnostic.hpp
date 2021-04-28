/// \file Compiler diagnostics, basically this is either a warning or an error
/// and this represents some place in the with some message linked to it.

#pragma once

#include "../lexer/token.hpp"
#include "../cmd.hpp"
#include <vector>
#include <string>

enum class diagnostic_type 
{
    global_warning,
    global_err,
    line_warning,
    line_err,
    location_warning,
    location_err
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

    // Writes the diagnostic, in a human readable way
    // Possible Improvement: Add an option to create a "machine-readable" error prompt
    std::string show_output(class parser const& par, std::vector<std::string> const& source, cmd& options) const;
};
