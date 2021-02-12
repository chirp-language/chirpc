#include "cmd.hpp"

#include <string>
#include <iostream> //temp

cmd parse_cmd(int argc, char* argv[])
{
    cmd c;
    if(argc == 1){
        c.error = true;
        std::cout<<"Not Enough Arguments"<<std::endl;
        return c;
    }
    for(int i = 1; i < argc; i++){
        std::string arg(argv[i]);

        if(arg == "-v"){
            c.verbose = true;
        }
        else if(arg == "--help"){
            c.help = true;
            return c;
        }
        else
        {
            if(c.filename.empty()){
                c.filename = arg;
            }
            else{
                std::cout<<"Unknown command argument \""<<arg<<"\""<<std::endl;
                c.error = true;
            }
        }
    }

    if(c.filename.empty() && c.help == false){
        c.error = true;
    }
    return c;
}