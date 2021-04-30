/*
This does all the command line parsing stuff, and then
*/
#pragma once

#include <string>

// Command line options
class cmd
{
public:
    // State information
    bool error = false;
    bool verbose = false;
    // Special commands
    bool help = false;
    bool version = false;
    // Options
    bool dump_tkns = false;
    bool dump_ast = false;
    bool keep_tmp = false;
    bool has_color = true;

    std::string filename;
    void write_help();
    void write_version();

    // shhhhhh
    static cmd* program_cmd;
};

cmd parse_cmd(int, char*[]);
