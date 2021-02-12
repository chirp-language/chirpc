// Base class for anything that has a location to it
#pragma once
#include <string>
class location
{
    public:
    // Improvement: Make a lookup table for the filenames or something
    // so it doesn't have to be stored in a full string, which is inneficient.
    std::string filename;

    int start; // Location of the first character
    int line; // Line where the first character is located
    int end; // Location of the last character
};