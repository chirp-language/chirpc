/// \file AST dumper for debug output

#pragma once

#include "ast.hpp"
#include "../shared/text_dumper_base.hpp"

class text_ast_dumper : public text_dumper_base {
    bool show_expr_types;
    int depth = 0;
    location_provider* loc_prov;

    public:
    text_ast_dumper(bool enable_colors, bool show_expr_types, location_provider* loc_prov = nullptr)
        : text_dumper_base(enable_colors), show_expr_types(show_expr_types), loc_prov(loc_prov)
    {}

    void dump_ast(ast_root const& root);
    void dump_identifier(identifier const&);
    void dump_qual_identifier(qual_identifier const&);
    void dump_expr(expr const& node);
    void dump_decl(decl const& node);
    void dump_stmt(stmt const& node);

    // Expressions
    void dump_basic_type(basic_type const&);
    void dump_expr_type(basic_type const& type, exprcat cat);
    void dump_binop(binop const&);
    void dump_unop(unop const&);
    void dump_arguments(arguments const&);
    void dump_func_call(func_call const&);
    void dump_id_ref_expr(id_ref_expr const&);
    void dump_loperand(loperand const&) = delete;
    void dump_string_literal(string_literal const&);
    void dump_integral_literal(integral_literal const&);
    void dump_nullptr_literal(nullptr_literal const&);
    void dump_cast_expr(cast_expr const&);
    // Declarations
    void dump_var_decl(var_decl const&);
    void dump_entry_decl(entry_decl const&);
    void dump_import_decl(import_decl const&);
    void dump_extern_decl(extern_decl const&);
    void dump_namespace_decl(namespace_decl const&);
    void dump_parameters(parameters const&);
    void dump_func_decl(func_decl const&);
    void dump_func_def(func_def const&);
    // Statements
    void dump_decl_stmt(decl_stmt const&);
    void dump_assign_stmt(assign_stmt const&);
    void dump_compound_stmt(compound_stmt const&);
    void dump_ret_stmt(ret_stmt const&);
    void dump_conditional_stmt(conditional_stmt const&);
    void dump_iteration_stmt(iteration_stmt const&);
    void dump_expr_stmt(expr_stmt const&);
    void dump_null_stmt(stmt const&); // null_stmt contains no further members

    private:
    void print_location(location_range);
};
