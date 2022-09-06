/*
I don't want to rely on the C++ STL for this, mostly because I can't manage to update my libc++, and filesystem is still
in the Experimental folder.
*/
#pragma once

#include <string>

namespace fs
{
    bool create_folder(std::string);
    bool write_file(std::string,std::string);
    bool remove_folder(std::string);
};

// Returns 0 on success, -(errno) on failure
int read_file_to_string(const char* fname, std::string& out);
