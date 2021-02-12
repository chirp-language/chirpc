// This manages to pretty much interface all components with eachothers
#include "cmd.hpp"
#include "lexer/lexer.hpp"
#include "lexer/preproc.hpp"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>

// Returns 0 if everything goes normal
// Returns -1 if bad code
// Returns -2 if error(i.e. can't open file)
int main(int argc, char** argv)
{
    cmd options = parse_cmd(argc,argv);
    
    if(options.error){
        return -1;
    }

    // Lets do the reading here cuz why the f not
    // Also kinda like very inefficient
    std::fstream f(options.filename);

    if(!f){
        std::cout<<"Can't open file: \""<<options.filename<<"\""<<std::endl;
        return -1;
    }

    //Doesn't need to use HackySTL to be hacky :^)
    std::vector<std::string> content;
    std::string line;
    while(std::getline(f,line)){
        content.push_back(line);
    }

    // Preprocessing & Lexing
    std::vector<location> proccesed = preprocess(content);
    std::vector<token> tkns = lexe(proccesed,content);
    for(token t:tkns){
        std::cout<<t.util_dump()<<std::endl;
    }
    // Parsing

    // AST Generation

    // Code Generation

    // Tooling

    return 0;
}