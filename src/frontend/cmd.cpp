#include "cmd.hpp"

#include <cstring>
#include <iostream> //temp

void cmd::write_help()
{
    std::cout <<
        "Usage: chirpc [options] file..\n"
        "\nOptions:\n"
        "--- User Options ---\n"
        "\t--help\tDisplays this help message\n"
        "\t--version\tDisplays the compiler version\n"
        "\t-v\tActivates verbose mode (Experimental)\n"
        "\t-cache\tOnly parses file and caches it\n"
        "\t-no-coloring\tDisables the coloring in prompts\n"
        "--- Utility Options ---\n"
        "\t-dump-tokens\tDumps the lexer tokens of the source file\n"
        "\t-dump-ast\tDumps the AST in a human readable view\n"
        "\t-dump-syms\tDumps the symbol table of the program\n"
        "\t-dump-syms-all\tDumps the symbol table of the program, including local and unnamed symbols\n"
        "\t-keep-tmp\tKeeps the temporary folder, instead of deleting it after compiling\n"
        "\t-no-out-gen\tDon't emit an output file, only check program for correctness\n"
        "\t-show-unresolved-refs\tShow warnings whether an undefined symbol is referenced "
            "(no longer needed since semantic analysis already reports errors; currently no-op)\n"
        "\t-show-expr-types\tShow types in expressions (effective during an AST dump)\n"
        "\t-soft-type-checks\tDon't generate errors on type mismatches\n"
    ;
}

void cmd::write_version()
{
    std::cout <<
        "chirpc (Unknown) unstable v0\n"
        "This version screen is temporary\n";
}

cmd parse_cmd(int argc, char *argv[])
{
    cmd c;
    if (argc == 1)
    {
        c.error = true;
        std::cout << "Not Enough Arguments\n";
        return c;
    }

    for (int i = 1; i < argc; i++)
    {
        std::string_view arg(argv[i]);
        if (arg == "--help")
        {
            c.help = true;
            return c;
        }
        else if (arg == "--version")
        {
            c.version = true;
            return c;
        }
        else if (arg == "-v")
        {
            // Currently verbose mode doesn't do anything
            c.verbose = true;
        }
        else if (arg == "-no-coloring")
        {
            c.has_color = false;
        }
        else if (arg == "-cache")
        {
            c.cache = true;
        }
        else if (arg == "-dump-tokens")
        {
            c.dump_tkns = true;
        }
        else if (arg == "-dump-ast")
        {
            c.dump_ast = true;
        }
        else if (arg == "-dump-syms")
        {
            c.dump_syms = true;
            c.dump_syms_extra = false;
        }
        else if (arg == "-dump-syms-all")
        {
            c.dump_syms = true;
            c.dump_syms_extra = true;
        }
        else if (arg == "-keep-tmp")
        {
            c.keep_tmp = true;
        }
        else if (arg == "-no-out-gen")
        {
            c.no_outgen = true;
        }
        else if (arg == "-show-unresolved-refs")
        {
            c.ignore_unresolved_refs = false;
        }
        else if (arg == "-show-expr-types")
        {
            c.show_expr_types = true;
        }
        else if (arg == "-soft-type-checks")
        {
            c.soft_type_checks = true;
        }
        else
        {
            if (c.filename.empty())
            {
                c.filename = argv[i];
            }
            else
            {
                std::cout << "Unknown command argument \"" << argv[i] << "\"\n";
                c.error = true;
            }
        }
    }

    if (c.filename.empty() && c.help == false)
    {
        c.error = true;
    }
    return c;
}
