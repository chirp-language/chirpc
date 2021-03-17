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

    dtypename get_dtypename(std::string);
    dtypemod get_dtypemod(std::string);

    bool is_operand(bool);

    bool is_identifier(bool);
    bool is_lop(bool); // left operand
    bool is_lvalue(bool);
    bool is_datatype(bool);

    bool is_params(bool);

    bool is_var_decl(bool); // (data specifiers) (:) (identifier)
    bool is_var_def(bool);  // (identifier) (=) (value)
    bool is_var_decldef(); // (data specifiers) (:) (identifier) (=) (value)

    bool is_func_decl(bool); // (func) (data_types) (identifier) (params)
    bool is_func_def(bool); // (func_decl) (compound_statement)
    bool is_func_call(bool); // (identifier) ( arguments )

    identifier get_identifier();
    lvalue get_lvalue();

    txt_literal get_txt_lit();
    num_literal get_num_lit();
    std::shared_ptr<literal_node> get_literal();

    subexpr get_subexpr(std::vector<operand>);
    operand get_operand();
    expr get_expr(std::vector<operand>);
    expr get_expr();

    dtype get_datatype();
    decl_stmt get_decl_stmt();
    def_stmt get_def_stmt();
    decldef_stmt get_decldef_stmt();

    parameters get_parameters();
    func_decl_stmt get_func_decl();
    func_def_stmt get_func_def();

    arguments get_arguments();
    func_call_stmt get_fcall(); // fcall->function call

    entry_stmt get_entry();
    import_stmt get_import();
    ret_stmt get_ret(); // MLG wooo

    extern_stmt get_extern();

    std::shared_ptr<stmt> get_stmt();
    compound_stmt get_compound_stmt();

    bool ok = false;
    std::string filename;
    ast tree;
    std::vector<helper> helpers;
    std::vector<token> tkns;
};