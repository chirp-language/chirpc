/*
I don't want to rely on the C++ STL for this, mostly because I can't manage to update my libc++, and filesystem is still
in the Experimental folder.
*/
#pragma once

#include <string>

namespace fs
{
    bool create_folder(std::string);

    bool remove_folder(std::string);
};