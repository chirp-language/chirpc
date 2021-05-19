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
    bool probe_range(tkn_type begin, tkn_type end); // begin <= probe() <= end
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

    bool is_binop();
    bool is_identifier();
    bool is_lop(); // left operand
    bool is_lvalue();
    bool is_datatype();
    bool is_datamod();
    bool is_type();

    bool is_params();

    bool is_var_decl(); // (data specifiers) (:) (identifier)
    bool is_var_assign();  // (identifier) (=) (value)

    // Expression stuff
    dtypename get_dtypename(std::string const&);
    dtypemod get_dtypemod(std::string const&);
    exprtype get_datatype();

    identifier get_identifier();

    txt_literal get_txt_lit();
    num_literal get_num_lit();
    nodeh<literal_node> get_literal();

    exprh get_subexpr_op(exprh lhs, int min_prec);
    exprh get_primary_expr();
    exprh get_expr(bool comma_allowed);

    arguments get_arguments();
    nodeh<func_call> get_fcall(exprh callee); // function call

    // Declaration stuff
    nodeh<entry_decl> get_entry();
    nodeh<import_decl> get_import();
    nodeh<extern_decl> get_extern();

    nodeh<var_decl> get_var_decl();
    nodeh<var_decl> get_parameter();

    nodeh<func_decl> get_func_decl();
    parameters get_parameters();

    // Statement stuff
    nodeh<stmt> get_stmt();
    nodeh<compound_stmt> get_compound_stmt();

    nodeh<decl_stmt> get_decl_stmt();
    nodeh<assign_stmt> get_assign_stmt();
    nodeh<ret_stmt> get_ret();
    nodeh<conditional_stmt> get_cond(); // 420 NoScope!
    nodeh<iteration_stmt> get_iter();

    bool ok = false;
    std::string filename;
    ast_root tree;
    diagnostic_manager& diagnostics;
    std::vector<token> tkns;
};
