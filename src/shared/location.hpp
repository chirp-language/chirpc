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

    int start = 0; // Location of the first character
    int line = 0; // Line where the first character is located
    int end = 0; // Location of the last character
};
