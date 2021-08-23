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
        "\t-v\tActivates verbose mode(Experimental)\n"
        "\t-no-coloring\tDisables the coloring in prompts\n"
        "--- Utility Options ---\n"
        "\t-dump-tokens\tDumps the lexer tokens of the source file\n"
        "\t-dump-ast\tDumps the AST in a human readable view\n"
        "\t-dump-syms\tDumps the symbol table of the program\n"
        "\t-dump-syms-all\tDumps the symbol table of the program, including local and unnamed symbols\n"
        "\t-keep-tmp\tKeeps the temporary folder, instead of deleting it after compiling\n"
        "\t-show-unresolved-refs\tShow warnings whether an undefined symbol is referenced"
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
        if (std::strcmp(argv[i], "--help") == 0)
        {
            c.help = true;
            return c;
        }
        else if (std::strcmp(argv[i], "--version") == 0)
        {
            c.version = true;
            return c;
        }
        else if (std::strcmp(argv[i], "-v") == 0)
        {
            // Currently verbose mode doesn't do anything
            c.verbose = true;
        }
        else if (std::strcmp(argv[i], "-no-coloring") == 0)
        {
            c.has_color = false;
        }
        else if (std::strcmp(argv[i], "-dump-tokens") == 0)
        {
            c.dump_tkns = true;
        }
        else if (std::strcmp(argv[i], "-dump-ast") == 0)
        {
            c.dump_ast = true;
        }
        else if (std::strcmp(argv[i], "-dump-syms") == 0)
        {
            c.dump_syms = true;
            c.dump_syms_extra = false;
        }
        else if (std::strcmp(argv[i], "-dump-syms-all") == 0)
        {
            c.dump_syms = true;
            c.dump_syms_extra = true;
        }
        else if (std::strcmp(argv[i], "-keep-tmp") == 0)
        {
            c.keep_tmp = true;
        }
        else if (std::strcmp(argv[i], "-show-unresolved-refs") == 0)
        {
            c.ignore_unresolved_refs = false;
        }
        else if (std::strcmp(argv[i], "-show-expr-types") == 0)
        {
            c.show_expr_types = true;
        }
        else if (std::strcmp(argv[i], "-soft-type-checks") == 0)
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
