#include "cmd.hpp"

#include <cstring>
#include <iostream> //temp

cmd parse_cmd(int argc, char* argv[])
{
    cmd c;
    if(argc == 1) {
        c.error = true;
        std::cout << "Not Enough Arguments\n";
        return c;
    }
    for(int i = 1; i < argc; i++) {

        if(std::strcmp(argv[i], "-v") == 0){
            c.verbose = true;
        }
        else if(std::strcmp(argv[i], "-dump-tokens") == 0){
            c.dump_tkns = true;
        }
        else if(std::strcmp(argv[i], "-dump-ast") == 0){
            c.dump_ast = true;
        }
        else if(std::strcmp(argv[i], "--help") == 0){
            c.help = true;
            return c;
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