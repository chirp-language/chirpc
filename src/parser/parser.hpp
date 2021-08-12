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
    void parse_top_level();

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

    bool is_datatype();
    bool is_datamod();
    bool is_type();
    bool is_var_decl(); // (data specifiers) (:) (identifier)

    // Expression stuff
    basic_type parse_datatype();

    identifier parse_identifier();
    qual_identifier parse_qual_identifier();

    string_literal build_string_lit(token_location loc, std::string&& value);
    integral_literal build_integral_lit(token_location loc, integer_value value, dtypename type);
    integral_literal build_bool_lit(token_location loc, bool value);
    nullptr_literal build_null_ptr_lit(token_location loc);
    exprh parse_str_or_char_lit(token_location loc, std::string const& tok_value);
    exprh parse_literal();

    exprh parse_subexpr_op(exprh lhs, int min_prec);
    exprh parse_unary_expr();
    exprh parse_primary_expr();
    exprh parse_expr(bool comma_allowed);

    arguments parse_arguments();
    nodeh<func_call> parse_fcall(exprh callee); // function call

    // Declaration stuff
    nodeh<entry_decl> parse_entry();
    nodeh<import_decl> parse_import();
    nodeh<extern_decl> parse_extern();
    nodeh<namespace_decl> parse_namespace();

    nodeh<var_decl> parse_var_decl();
    nodeh<var_decl> parse_parameter();

    nodeh<func_decl> parse_func_decl();
    parameters parse_parameters();

    // Statement stuff
    nodeh<stmt> parse_stmt();
    nodeh<compound_stmt> parse_compound_stmt();

    nodeh<decl_stmt> parse_decl_stmt();
    nodeh<assign_stmt> parse_assign_stmt(exprh target);
    nodeh<ret_stmt> parse_ret();
    nodeh<conditional_stmt> parse_cond(); // 420 NoScope!
    nodeh<iteration_stmt> parse_iter();

    bool ok = false;
    std::string filename;
    ast_root tree;
    diagnostic_manager& diagnostics;
    std::vector<token> tkns;
};
