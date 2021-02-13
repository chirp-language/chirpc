/*
Parses tokens into an AST
*/
#pragma once
#include "../shared/helper.hpp"
#include "../lexer/token.hpp"
#include "../ast/ast.hpp"
#include <vector>

// (bootleg)Parser  
class parser
{
    public:
    void parse();

    void load_tokens(std::string,std::vector<token>);
    std::vector<helper> get_helpers();
    ast get_ast();
    private:
    size_t cursor = 0;

    // Shared functions

    bool match(tkn_type);
    bool expect(tkn_type);

    token peekb(); // Peek Back
    token peek(); // Peek now
    token peekf(); // Peek Forward

    // Actual parser stuff

    bool ok = false;
    std::string filename;
    ast tree;
    std::vector<helper> helpers;
    std::vector<token> tkns;
};