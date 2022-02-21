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
    bool dump_syms = false;
    bool dump_syms_extra = false;
    bool keep_tmp = false;
    bool no_outgen = false;
    bool has_color = true;
    bool ignore_unresolved_refs = true;
    bool show_expr_types = false;
    bool soft_type_checks = false;

    std::string filename;
    void write_help();
    void write_version();
};

cmd parse_cmd(int, char*[]);
