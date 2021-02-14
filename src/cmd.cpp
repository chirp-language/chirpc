#include "cmd.hpp"

#include <cstring>
#include <iostream> //temp

void cmd::write_help()
{
    std::cout<<"Usage: chirpc [options] file..\n";
    std::cout<<"\nOptions:\n";
    std::cout<<"--- User Options ---\n";
    std::cout<<"\t--help\tDisplays this help message\n";
    std::cout<<"\t--version\tDisplays the compiler version\n";
    std::cout<<"\t-v\tActivates verbose mode(Experimental)\n";
    std::cout<<"\t-no-coloring\tDisables the coloring in prompts\n";
    std::cout<<"--- Utility Options ---\n";
    std::cout<<"\t-dump-tokens\tDumps the lexer tokens of the source file\n";
    std::cout<<"\t-dump-ast\tDumps the AST in a human readable view\n";
}

void cmd::write_version()
{
    std::cout<<"chirpc (Unknown) unstable v0\n";
    std::cout<<"This version screen is temporary\n";
}

cmd parse_cmd(int argc, char* argv[])
{
    cmd c;
    if(argc == 1) {
        c.error = true;
        std::cout << "Not Enough Arguments\n";
        return c;
    }
    for(int i = 1; i < argc; i++) {
        if(std::strcmp(argv[i], "--help") == 0){
            c.help = true;
            return c;
        }
        else if(std::strcmp(argv[i],"--version") == 0){
            c.version = true;
            return c;
        }
        else if(std::strcmp(argv[i], "-v") == 0){
            // Currently verbose mode doesn't do anything
            c.verbose = true;
        }
        else if(std::strcmp(argv[i],"-no-coloring") == 0){
            c.has_color = false;
        }
        else if(std::strcmp(argv[i], "-dump-tokens") == 0){
            c.dump_tkns = true;
        }
        else if(std::strcmp(argv[i], "-dump-ast") == 0){
            c.dump_ast = true;
        }
        else
        {
            if(c.filename.empty()) {
                c.filename = argv[i];
            }
            else {
                std::cout << "Unknown command argument \"" << argv[i] << "\"\n";
                c.error = true;
            }
        }
    }

    if(c.filename.empty() && c.help == false) {
        c.error = true;
    }
    return c;
}