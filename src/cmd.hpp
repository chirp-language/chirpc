/*
This does all the command line parsing stuff, and then
*/
#pragma once

#include <string>

// Command line options
class cmd
{
    public:
    bool error = false;
    bool verbose = false;
    bool help = false;
    std::string filename;
};

cmd parse_cmd(int,char*[]);