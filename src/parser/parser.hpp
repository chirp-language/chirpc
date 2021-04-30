/*
Parses tokens into an AST
*/
#pragma once
#include "../shared/diagnostic.hpp"
#include "../shared/location_provider.hpp"
#include "../lexer/token.hpp"
#include "../ast/ast.hpp"
#include <vector>

// (bootleg)Parser  
class parser : public location_provider
{
public:
    void parse();

    void load_tokens(std::string, std::vector<token>&&);
    std::vector<diagnostic> const& get_diagnostics() const
    {
        return diagnostics;
    }
    ast_root& get_ast()
    {
        return tree;
    }
    std::vector<token> const& get_tokens() const
    {
        return tkns;
    }

    location const& get_loc(token_location loc) const override;

private:
    size_t cursor = 0;

    // Shared functions

    bool match(tkn_type);
    bool probe(tkn_type); // Like match, but not consuming
    bool expect(tkn_type);

    token_location loc_peek()
    {
        return token_location(cursor);
    }

    token_location loc_peekb()
    {
        return token_location(cursor - 1);
    }

    token_location loc_peekf()
    {
        if (cursor + 1 >= tkns.size())
            return token_location();
        return token_location(cursor + 1);
    }

    token_location loc_eof()
    {
        // Returns the location of the last token, which should be EOF
        return token_location(tkns.size() - 1);
    }

    token const& peekb(); // Peek Back
    token const& peek(); // Peek now
    token const& peekf(); // Peek Forward

    void skip() { ++cursor; } // Skip one token

    // Actual parser stuff

    dtypename get_dtypename(std::string);
    dtypemod get_dtypemod(std::string);

    bool is_operand();

    bool is_identifier();
    bool is_lop(); // left operand
    bool is_lvalue();
    bool is_datatype();

    bool is_params();

    bool is_var_decl(); // (data specifiers) (:) (identifier)
    bool is_var_def();  // (identifier) (=) (value)
    bool is_var_decldef(); // (data specifiers) (:) (identifier) (=) (value)

    bool is_func_decl(); // (func) (data_types) (identifier) (params)
    bool is_func_def(); // (func_decl) (compound_statement)
    bool is_func_call(); // (identifier) ( arguments )

    std::shared_ptr<identifier> get_identifier();

    txt_literal get_txt_lit();
    num_literal get_num_lit();
    std::shared_ptr<literal_node> get_literal();

    exprh get_subexpr_op(exprh lhs, int min_prec);
    exprh get_primary_expr();
    exprh get_expr(bool comma_allowed);

    exprtype get_datatype();
    std::shared_ptr<decl_stmt> get_decl_stmt();
    std::shared_ptr<def_stmt> get_def_stmt();

    parameters get_parameters();
    std::shared_ptr<func_decl_stmt> get_func_decl();
    std::shared_ptr<func_def_stmt> get_func_def();

    arguments get_arguments();
    std::shared_ptr<func_call> get_fcall(exprh callee); // function call

    entry_stmt get_entry();
    std::shared_ptr<import_stmt> get_import();
    std::shared_ptr<ret_stmt> get_ret(); // MLG wooo

    std::shared_ptr<extern_stmt> get_extern();

    std::shared_ptr<stmt> get_stmt();
    std::shared_ptr<compound_stmt> get_compound_stmt();

    bool ok = false;
    std::string filename;
    ast_root tree;
    std::vector<diagnostic> diagnostics;
    std::vector<token> tkns;
};
