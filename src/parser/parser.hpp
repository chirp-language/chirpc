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
    ast_root& get_ast()
    {
        return tree;
    }
    std::vector<token> const& get_tokens() const
    {
        return tkns;
    }

    location const& get_loc(token_location loc) const override;

    parser(diagnostic_manager& diag)
        : diagnostics(diag) {}
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

    bool is_operand();

    bool is_identifier();
    bool is_lop(); // left operand
    bool is_lvalue();
    bool is_datatype();

    bool is_params();

    bool is_var_decl(); // (data specifiers) (:) (identifier)
    bool is_var_assign();  // (identifier) (=) (value)

    // Expression stuff
    dtypename get_dtypename(std::string const&);
    dtypemod get_dtypemod(std::string const&);
    exprtype get_datatype();

    std::shared_ptr<identifier> get_identifier();

    txt_literal get_txt_lit();
    num_literal get_num_lit();
    std::shared_ptr<literal_node> get_literal();

    exprh get_subexpr_op(exprh lhs, int min_prec);
    exprh get_primary_expr();
    exprh get_expr(bool comma_allowed);

    arguments get_arguments();
    std::shared_ptr<func_call> get_fcall(exprh callee); // function call

    // Declaration stuff
    std::shared_ptr<entry_decl> get_entry();
    std::shared_ptr<import_decl> get_import();
    std::shared_ptr<extern_decl> get_extern();
    std::shared_ptr<namespace_decl> get_namespace();

    std::shared_ptr<var_decl> get_var_decl();
    std::shared_ptr<var_decl> get_parameter();

    std::shared_ptr<func_decl> get_func_decl();
    parameters get_parameters();

    // Statement stuff
    std::shared_ptr<stmt> get_stmt();
    std::shared_ptr<compound_stmt> get_compound_stmt();

    std::shared_ptr<decl_stmt> get_decl_stmt();
    std::shared_ptr<assign_stmt> get_assign_stmt();
    std::shared_ptr<ret_stmt> get_ret();
    std::shared_ptr<conditional_stmt> get_cond(); // 420 NoScope!
    std::shared_ptr<iteration_stmt> get_iter();

    bool ok = false;
    std::string filename;
    ast_root tree;
    diagnostic_manager& diagnostics;
    std::vector<token> tkns;
};
