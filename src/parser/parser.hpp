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

    void load_tokens(std::string, std::vector<token>);
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

    bool is_func_call();

    identifier get_identifier();

    txt_literal get_txt_lit();
    num_literal get_num_lit();
    std::shared_ptr<literal_node> get_literal();

    arguments get_arguments();

    entry_stmt get_entry();
    import_stmt get_import();
    ret_stmt get_ret(); // MLG wooo

    func_call_stmt get_fcall(); // fcall->function call

    std::shared_ptr<stmt> get_stmt();
    compound_stmt get_compound_stmt();

    bool ok = false;
    std::string filename;
    ast tree;
    std::vector<helper> helpers;
    std::vector<token> tkns;
};