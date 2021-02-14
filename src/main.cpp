// This manages to pretty much interface all components with eachothers
#include "cmd.hpp"
#include "lexer/lexer.hpp"
#include "lexer/preproc.hpp"
#include "parser/parser.hpp"
#include "frontend/frontend.hpp"
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
        std::cout << "Error in provided arguments\n";
        return -1;
    }

    // Lets do the reading here cuz why the f not
    // Also kinda like very inefficient
    std::fstream f(options.filename);

    if(!f) {
        std::cout << "Can't open file: \"" << options.filename << "\"\n";
        return -1;
    }

    //Doesn't need to use HackySTL to be hacky :^)
    std::vector<std::string> content;
    std::string line;

    while(std::getline(f, line)) {
        content.push_back(line);
    }

    // Preprocessing & Lexing
    std::vector<location> proccesed = preprocess(options.filename, content);
    std::vector<token> tkns = lexe(proccesed, content);
    
    if(options.dump_tkns) {
        std::cout << "Tokens:\n";
        
        for(token& t : tkns) {
            std::cout << t.util_dump() << '\n';
        }
    }
    // Parsing
    parser p;
    p.load_tokens(options.filename, tkns);
    p.parse();
    std::vector<helper> phelpers = p.get_helpers();
    bool ok = true;

    for(helper& h : phelpers) {
        // Always copying the file content is like
        // really really really bad & inneficient
        std::cout << h.write_helper(content) << '\n';

        if(
            h.type==helper_type::global_err||
            h.type==helper_type::line_err||
            h.type==helper_type::location_err
        ){
            ok = false;
        }
    }

    if(options.dump_ast) {
        if(options.dump_tkns) {
            std::cout << "--------------------" << '\n';
        }
        std::cout << p.get_ast().dump() << '\n';
    }
    // Code Generation
    if(!ok){
        return -1;
    }

    frontend frontend;

    if(!frontend.find_compiler()){
        std::cout<<"Couldn't find supported C compiler on this machine.\n";
        std::cout<<"Supported compilers are clang-10 and gcc7.5.0\n";
        std::cout<<"The compiler may still work on earlier versions.\n";
        return -1;
    }

    frontend.make_tmp_folder();
    
    // Tooling

    // Cleanup
    frontend.remove_tmp_folder();

    return 0;
}