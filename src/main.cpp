// This manages to pretty much interface all components with eachothers

#include "cmd.hpp"

int main(int argc, char** argv)
{
    cmd options = parse_cmd(argc,argv);
    
    if(options.error){
        return -1;
    }

    return 0;
}