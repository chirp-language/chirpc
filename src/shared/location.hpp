// Base class for anything that has a location to it
#pragma once
#include <string>

class location
{
public:
    // Empty location
    location();

    // Empty location but you know the file
    location(std::string);

    // Creates a location with only a line
    location(int, std::string);
    // Improvement: Make a lookup table for the filenames or something
    // so it doesn't have to be stored in a full string, which is inneficient.
    std::string filename;

    int start = -1; // Location of the first character
    int line = -1; // Line where the first character is located
    int len = 0; // Length of the token
};

// State of location printer on a specific task, like AST dump, token dump, etc.
struct location_run
{
	location const* last_loc = nullptr;
};

// Stolen from clang
struct token_location
{
    ssize_t loc;

    explicit constexpr token_location() noexcept : loc(-1) {}
    explicit constexpr token_location(ssize_t l) noexcept : loc(l) {}

    bool is_valid() const noexcept { return loc >= 0; }

    constexpr bool operator==(token_location o) const noexcept
    { return loc == o.loc; }

    constexpr bool operator!=(token_location o) const noexcept
    { return loc != o.loc; }
};

struct location_range
{
    token_location begin, end;

    explicit constexpr location_range() noexcept {}
    constexpr location_range(token_location loc) noexcept : begin(loc), end(loc) {}
    constexpr location_range(token_location b, token_location e) noexcept
        : begin(b), end(e) {}
};

void print_loc_single(location const& loc, std::string& str, location_run* run);
